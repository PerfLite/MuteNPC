#include "Pch.h"
#include "DialogueHook.h"
#include "Config.h"
#include "RE/A/Actor.h"
#include "RE/A/AIProcess.h"
#include "RE/H/HighProcessData.h"
#include "RE/D/DialogueItem.h"
#include "RE/D/DialogueTypes.h"
#include "RE/T/TESTopic.h"
#include "RE/M/MenuTopicManager.h"
#include "RE/S/SubtitleManager.h"
#include "RE/B/BarterMenu.h"
#include "RE/D/DialogueMenu.h"
#include "RE/U/UI.h"
#include <cstring>

namespace MuteNPC
{
    using Ctor_t = RE::DialogueItem* (*)(
        RE::DialogueItem* a_this,
        RE::TESQuest* a_quest,
        RE::TESTopic* a_topic,
        RE::TESTopicInfo* a_topicInfo,
        RE::Actor* a_speaker);

    static inline Ctor_t _orig_Ctor = nullptr;

    // ------------------------------------------------------------------
    // Глушим звук голоса у активного актёра.
    // Останавливаем аудиоканалы в high process и ExtraSayToTopicInfo.
    // НЕ сбрасываем voiceTimer и не вызываем SetDialogueWithPlayer(false),
    // чтобы движок не считал, что приветствие прервано.
    // ------------------------------------------------------------------
    static bool SilenceVoice(RE::Actor* a_actor)
    {
        if (!a_actor) return false;
        bool stopped = false;

        try {
            auto* process = a_actor->GetActorRuntimeData().currentProcess;
            if (process && process->high) {
                for (auto& h : process->high->soundHandles) {
                    if (h.IsValid() && h.IsPlaying()) {
                        h.SetVolume(0.0f);
                        h.Stop();
                        stopped = true;
                    }
                }
            }

            auto* extra = a_actor->extraList.GetByType<RE::ExtraSayToTopicInfo>();
            if (extra && extra->unk28.IsValid() && extra->unk28.IsPlaying()) {
                extra->unk28.SetVolume(0.0f);
                extra->unk28.Stop();
                stopped = true;
            }
        } catch (...) {}

        return stopped;
    }

    void StopSpeech(RE::Actor* a_actor)
    {
        if (SilenceVoice(a_actor) && Config::GetSingleton().GetSettings().blockSubtitles) {
            try {
                if (auto* sm = RE::SubtitleManager::GetSingleton()) {
                    sm->KillSubtitles();
                }
            } catch (...) {}
        }
    }

    static bool ShouldSilence(RE::Actor* a_actor)
    {
        if (!a_actor || a_actor->IsPlayerRef() || a_actor->IsDead()) return false;

        auto& cfgMgr = Config::GetSingleton();
        const auto& cfg = cfgMgr.GetSettings();
        if (!cfg.enabled || !cfg.muteVoiceAudio) return false;
        if (!cfgMgr.IsMuted(a_actor)) return false;

        return true;
    }

    static RE::DialogueItem* Hook_DialogueItem_Ctor(
        RE::DialogueItem* a_this,
        RE::TESQuest* a_quest,
        RE::TESTopic* a_topic,
        RE::TESTopicInfo* a_topicInfo,
        RE::Actor* a_speaker)
    {
        auto* item = _orig_Ctor(a_this, a_quest, a_topic, a_topicInfo, a_speaker);
        if (!item) return item;

        auto* actor = a_speaker;
        if (!actor && item->speaker) {
            actor = item->speaker->As<RE::Actor>();
        }
        if (!actor) {
            auto* mtm = RE::MenuTopicManager::GetSingleton();
            if (mtm) {
                if (auto spk = mtm->speaker.get()) {
                    actor = spk->As<RE::Actor>();
                }
                if (!actor) {
                    if (auto lastSpk = mtm->lastSpeaker.get()) {
                        actor = lastSpk->As<RE::Actor>();
                    }
                }
            }
        }
        if (!actor) return item;

        const auto& cfg = Config::GetSingleton().GetSettings();
        if (!cfg.enabled) return item;

        if (ShouldSilence(actor)) {
            for (auto* resp : item->responses) {
                if (!resp) continue;

                if (cfg.muteVoiceAudio) {
                    resp->voiceSound = nullptr;
                    resp->voice = RE::BSFixedString("");
                }

                if (cfg.blockLipSync) {
                    resp->soundLip = false;
                    resp->speakerIdle = nullptr;
                    resp->useEmotion = false;
                }

                if (cfg.blockSubtitles) {
                    resp->text = "";
                }
            }

            SilenceVoice(actor);
        }

        return item;
    }

    class DialogueMenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        static DialogueMenuWatcher& GetSingleton()
        {
            static DialogueMenuWatcher s_instance;
            return s_instance;
        }

        void Install()
        {
            auto* ui = RE::UI::GetSingleton();
            if (ui) {
                ui->AddEventSink<RE::MenuOpenCloseEvent>(this);
                SKSE::log::info("MuteNPC: Registered MenuOpenCloseEvent watcher");
            }
        }

        RE::BSEventNotifyControl ProcessEvent(
            const RE::MenuOpenCloseEvent* a_event,
            RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
        {
            if (!a_event) return RE::BSEventNotifyControl::kContinue;

            // When DialogueMenu or BarterMenu opens OR closes: immediately silence the active speaker if muted
            if (a_event->menuName == RE::DialogueMenu::MENU_NAME || 
                a_event->menuName == RE::BarterMenu::MENU_NAME) 
            {
                auto* mtm = RE::MenuTopicManager::GetSingleton();
                if (mtm) {
                    auto h1 = mtm->speaker;
                    auto h2 = mtm->lastSpeaker;
                    auto r1 = h1.get();
                    auto r2 = h2.get();
                    if (r1) {
                        auto* a1 = r1->As<RE::Actor>();
                        if (a1 && Config::GetSingleton().IsMuted(a1)) {
                            StopSpeech(a1);
                        }
                    }
                    if (r2 && r2 != r1) {
                        auto* a2 = r2->As<RE::Actor>();
                        if (a2 && Config::GetSingleton().IsMuted(a2)) {
                            StopSpeech(a2);
                        }
                    }
                }
            }

            return RE::BSEventNotifyControl::kContinue;
        }
    };

    void InstallHooks()
    {
        try {
            auto& trampoline = SKSE::GetTrampoline();
            REL::Relocation<uintptr_t> ctorTarget{ RELOCATION_ID(34413, 35220) };
            const uintptr_t targetAddr = ctorTarget.address();

            // Stolen prologue (7 bytes):
            // 48 8B C4        mov rax, rsp
            // 48 89 48 08     mov [rax+8], rcx
            constexpr uint8_t stolenBytes[] = { 0x48, 0x8B, 0xC4, 0x48, 0x89, 0x48, 0x08 };
            constexpr size_t stolenSize = sizeof(stolenBytes);

            // Allocate executable memory in SKSE trampoline (7 bytes stolen + 14 bytes absolute jmp = 21 bytes)
            auto* mem = static_cast<uint8_t*>(trampoline.allocate(stolenSize + 14));
            if (!mem) {
                SKSE::log::critical("MuteNPC: Failed to allocate trampoline memory for DialogueItem detour");
                return;
            }

            // 1. Copy original stolen instructions
            std::memcpy(mem, stolenBytes, stolenSize);

            // 2. Add 64-bit indirect jump back to (targetAddr + stolenSize):
            // FF 25 00 00 00 00 [8 bytes 64-bit address]
            mem[stolenSize + 0] = 0xFF;
            mem[stolenSize + 1] = 0x25;
            mem[stolenSize + 2] = 0x00;
            mem[stolenSize + 3] = 0x00;
            mem[stolenSize + 4] = 0x00;
            mem[stolenSize + 5] = 0x00;
            const uintptr_t resumeAddr = targetAddr + stolenSize;
            std::memcpy(mem + stolenSize + 6, &resumeAddr, sizeof(resumeAddr));

            _orig_Ctor = reinterpret_cast<Ctor_t>(mem);

            // 3. Write 5-byte JMP to Hook_DialogueItem_Ctor at targetAddr
            trampoline.write_branch<5>(targetAddr, reinterpret_cast<uintptr_t>(Hook_DialogueItem_Ctor));

            // 4. Pad remaining 2 bytes of the 7-byte overwritten region with NOPs
            constexpr uint8_t nops[2] = { 0x90, 0x90 };
            REL::safe_write(targetAddr + 5, nops, sizeof(nops));

            DialogueMenuWatcher::GetSingleton().Install();

            SKSE::log::info("MuteNPC: DialogueItem::Ctor detour hook and DialogueMenuWatcher installed successfully at 0x{:X}", targetAddr);
        } catch (const std::exception& e) {
            SKSE::log::error("MuteNPC: Failed to install hooks: {}", e.what());
        } catch (...) {
            SKSE::log::error("MuteNPC: Unknown exception while installing hooks");
        }
    }
}
