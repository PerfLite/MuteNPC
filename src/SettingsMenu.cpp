#include "Pch.h"
#include "SettingsMenu.h"
#include "SKSEMenuFramework_API.h"
#include "Config.h"
#include "DialogueHook.h"
#include "I18n.h"
#include <cstdio>

namespace MuteNPC
{
    static RE::Actor* GetCrosshairActor()
    {
        auto* crosshair = RE::CrosshairPickData::GetSingleton();
        if (!crosshair) return nullptr;

        auto handle = crosshair->targetActor ? crosshair->targetActor : crosshair->target;
        auto refr = handle.get();
        if (refr) {
            return refr->As<RE::Actor>();
        }
        return nullptr;
    }

    void SettingsMenu::RenderUI()
    {
        auto& cfgMgr = Config::GetSingleton();
        auto& cfg = cfgMgr.GetSettings();
        const auto& tr = Tr();
        bool changed = false;

        SKSEMenuFramework::igText(tr.menu_header.c_str());
        SKSEMenuFramework::igSeparator();

        // 1. Главный переключатель
        if (SKSEMenuFramework::igCheckbox(tr.menu_enable.c_str(), &cfg.enabled)) {
            changed = true;
        }

        SKSEMenuFramework::igSeparator();

        // 2. Цель под прицелом
        SKSEMenuFramework::igText(tr.target_section.c_str());
        auto* target = GetCrosshairActor();
        if (target) {
            auto* base = target->GetActorBase();
            const char* name = base ? base->GetName() : target->GetName();
            std::string displayName = (name && name[0]) ? name : "NPC";

            char buf[128];
            std::snprintf(buf, sizeof(buf), tr.target_name.c_str(), displayName.c_str());
            SKSEMenuFramework::igText("%s", buf);

            bool isMuted = cfgMgr.IsMuted(target);
            if (isMuted) {
                if (SKSEMenuFramework::igButton(tr.target_btn_unmute.c_str(), { 0, 0 })) {
                    cfgMgr.UnmuteActor(target);
                    changed = true;
                }
            } else {
                if (SKSEMenuFramework::igButton(tr.target_btn_mute.c_str(), { 0, 0 })) {
                    cfgMgr.MuteActor(target);
                    StopSpeech(target);
                    changed = true;
                }
            }
        } else {
            SKSEMenuFramework::igText(tr.target_none.c_str());
        }

        SKSEMenuFramework::igSeparator();

        // 3. Параметры
        SKSEMenuFramework::igText(tr.params_section.c_str());
        if (SKSEMenuFramework::igCheckbox(tr.params_hide_subtitles.c_str(), &cfg.blockSubtitles)) {
            changed = true;
        }

        // Выбор языка (Слайдер / Выпадающий список)
        int langIdx = 0;
        if (cfg.language == "EN") langIdx = 1;
        else if (cfg.language == "RU") langIdx = 2;
        else langIdx = 0; // "auto"

        if (SKSEMenuFramework::igCombo_Str) {
            const char* items = "Auto (Skyrim / Игра)\0English (EN)\0Русский (RU)\0\0";
            if (SKSEMenuFramework::igCombo_Str(tr.params_language.c_str(), &langIdx, items, -1)) {
                if (langIdx == 1) cfg.language = "EN";
                else if (langIdx == 2) cfg.language = "RU";
                else cfg.language = "auto";

                I18n::GetSingleton().Load(cfg.language);
                changed = true;
            }
        } else if (SKSEMenuFramework::igSliderInt) {
            const char* fmt = (langIdx == 1) ? "English (EN)" : ((langIdx == 2) ? "Русский (RU)" : "Auto (Игра)");
            if (SKSEMenuFramework::igSliderInt(tr.params_language.c_str(), &langIdx, 0, 2, fmt, 0)) {
                if (langIdx == 1) cfg.language = "EN";
                else if (langIdx == 2) cfg.language = "RU";
                else cfg.language = "auto";

                I18n::GetSingleton().Load(cfg.language);
                changed = true;
            }
        }

        SKSEMenuFramework::igSeparator();

        // 4. Список заглушенных NPC
        char listHeader[128];
        std::snprintf(listHeader, sizeof(listHeader), tr.list_header.c_str(), cfg.mutedList.size());
        if (SKSEMenuFramework::igCollapsingHeader && SKSEMenuFramework::igCollapsingHeader(listHeader, 0)) {
            if (cfg.mutedList.empty()) {
                SKSEMenuFramework::igText(tr.list_empty.c_str());
            } else {
                size_t toRemove = static_cast<size_t>(-1);
                for (size_t i = 0; i < cfg.mutedList.size(); ++i) {
                    const auto& item = cfg.mutedList[i];

                    char btnDel[64];
                    std::snprintf(btnDel, sizeof(btnDel), "%s##%zu", tr.list_btn_delete.c_str(), i);
                    if (SKSEMenuFramework::igButton(btnDel, { 0, 0 })) {
                        toRemove = i;
                    }

                    SKSEMenuFramework::igSameLine(0.0f, 10.0f);

                    char itemBuf[128];
                    std::snprintf(itemBuf, sizeof(itemBuf), "#%zu: %s", i + 1, item.name.c_str());
                    SKSEMenuFramework::igText("%s", itemBuf);
                }

                if (toRemove != static_cast<size_t>(-1)) {
                    cfgMgr.RemoveAt(toRemove);
                    changed = true;
                }

                SKSEMenuFramework::igSeparator();
                if (SKSEMenuFramework::igButton(tr.list_btn_clear.c_str(), { 0, 0 })) {
                    cfg.mutedList.clear();
                    changed = true;
                }
            }
        }

        SKSEMenuFramework::igSeparator();

        // 5. Сброс настроек
        if (SKSEMenuFramework::igButton(tr.settings_btn_reset.c_str(), { 0, 0 })) {
            cfg.enabled = true;
            cfg.muteVoiceAudio = true;
            cfg.blockLipSync = false;
            cfg.blockSubtitles = true;
            cfg.language = "auto";
            cfg.mutedList.clear();
            I18n::GetSingleton().Load("auto");
            changed = true;
        }

        if (changed) {
            cfgMgr.Save();
        }
    }

    void SettingsMenu::Register()
    {
        bool smfOk = SKSEMenuFramework::Init();
        SKSE::log::info("MuteNPC: SKSEMenuFramework::Init() = {}", smfOk ? "SUCCESS" : "NOT FOUND");

        if (smfOk && SKSEMenuFramework::AddSectionItem) {
            SKSEMenuFramework::AddSectionItem(Tr().menu_section.c_str(), RenderUI);
            SKSE::log::info("MuteNPC: Registered section in SKSE Menu Framework: {}", Tr().menu_section);
        }
    }
}
