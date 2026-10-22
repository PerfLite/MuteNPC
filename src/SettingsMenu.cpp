#include "Pch.h"
#include "SettingsMenu.h"
#include "SKSEMenuFramework_API.h"
#include "Config.h"
#include "DialogueHook.h"
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
        bool changed = false;

        SKSEMenuFramework::igText("=== MuteNPC: Заглушение NPC ===");
        SKSEMenuFramework::igSeparator();

        // 1. Главный переключатель
        if (SKSEMenuFramework::igCheckbox("Включить мод", &cfg.enabled)) {
            changed = true;
        }

        SKSEMenuFramework::igSeparator();

        // 2. Цель под прицелом
        SKSEMenuFramework::igText("Цель под прицелом:");
        auto* target = GetCrosshairActor();
        if (target) {
            auto* base = target->GetActorBase();
            const char* name = base ? base->GetName() : target->GetName();
            std::string displayName = (name && name[0]) ? name : "NPC";

            char buf[128];
            std::snprintf(buf, sizeof(buf), "  Цель: %s", displayName.c_str());
            SKSEMenuFramework::igText("%s", buf);

            bool isMuted = cfgMgr.IsMuted(target);
            if (isMuted) {
                if (SKSEMenuFramework::igButton("Вернуть голос этой цели", { 0, 0 })) {
                    cfgMgr.UnmuteActor(target);
                    changed = true;
                }
            } else {
                if (SKSEMenuFramework::igButton("Заглушить эту цель", { 0, 0 })) {
                    cfgMgr.MuteActor(target);
                    StopSpeech(target);
                    changed = true;
                }
            }
        } else {
            SKSEMenuFramework::igText("  (наведите прицел на любого NPC)");
        }

        SKSEMenuFramework::igSeparator();

        // 3. Параметры
        SKSEMenuFramework::igText("Параметры:");
        if (SKSEMenuFramework::igCheckbox("Скрывать субтитры заглушенных фраз", &cfg.blockSubtitles)) {
            changed = true;
        }

        SKSEMenuFramework::igSeparator();

        // 4. Список заглушенных NPC
        char listHeader[128];
        std::snprintf(listHeader, sizeof(listHeader), "Список заглушенных NPC (%zu)", cfg.mutedList.size());
        if (SKSEMenuFramework::igCollapsingHeader && SKSEMenuFramework::igCollapsingHeader(listHeader, 0)) {
            if (cfg.mutedList.empty()) {
                SKSEMenuFramework::igText("Список пуст. Наведите прицел на NPC и нажмите «Заглушить эту цель».");
            } else {
                size_t toRemove = static_cast<size_t>(-1);
                for (size_t i = 0; i < cfg.mutedList.size(); ++i) {
                    const auto& item = cfg.mutedList[i];

                    char btnDel[64];
                    std::snprintf(btnDel, sizeof(btnDel), "Удалить##%zu", i);
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
                if (SKSEMenuFramework::igButton("Очистить весь список", { 0, 0 })) {
                    cfg.mutedList.clear();
                    changed = true;
                }
            }
        }

        SKSEMenuFramework::igSeparator();

        // 5. Сброс настроек
        if (SKSEMenuFramework::igButton("Сбросить настройки к стандартным", { 0, 0 })) {
            cfg.enabled = true;
            cfg.muteVoiceAudio = true;
            cfg.blockLipSync = false;
            cfg.blockSubtitles = true;
            cfg.mutedList.clear();
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
            SKSEMenuFramework::AddSectionItem("MuteNPC/Настройки", RenderUI);
            SKSE::log::info("MuteNPC: Registered section in SKSE Menu Framework");
        }
    }
}
