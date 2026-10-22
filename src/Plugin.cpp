#include "Pch.h"
#include "Config.h"
#include "DialogueHook.h"
#include "SettingsMenu.h"
#include "I18n.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
    void InitializeLogging()
    {
        auto path = SKSE::log::log_directory();
        if (!path) {
            return;
        }

        *path /= "MuteNPC.log";
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
        auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));

        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    }

    void OnMessage(SKSE::MessagingInterface::Message* a_msg)
    {
        if (!a_msg) return;

        switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            {
                SKSE::log::info("MuteNPC: kDataLoaded received, initializing...");

                try {
                    // Load translations
                    MuteNPC::I18n::GetSingleton().Load(MuteNPC::Config::GetSingleton().GetSettings().language);

                    // Install hooks
                    MuteNPC::InstallHooks();

                    // Register in SKSE Menu Framework
                    MuteNPC::SettingsMenu::Register();
                    SKSE::log::info("MuteNPC: Initialization complete!");
                } catch (const std::exception& e) {
                    SKSE::log::critical("MuteNPC: Exception during kDataLoaded: {}", e.what());
                } catch (...) {
                    SKSE::log::critical("MuteNPC: Unknown exception during kDataLoaded");
                }
            }
            break;
        default:
            break;
        }
    }
}

SKSEPluginInfo(
    .Version = REL::Version{ 1, 0, 0, 0 },
    .Name = "MuteNPC",
    .Author = "PerfLite"
)

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    InitializeLogging();
    SKSE::log::info("MuteNPC v1.0.0 loading...");

    try {
        auto* messaging = static_cast<SKSE::MessagingInterface*>(
            a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));

        if (!messaging) {
            SKSE::log::critical("MuteNPC: Failed to query SKSE messaging interface");
            return false;
        }

        SKSE::Init(a_skse);
        SKSE::AllocTrampoline(256);

        // Load configuration
        MuteNPC::Config::GetSingleton().Load();

        messaging->RegisterListener(OnMessage);

        SKSE::log::info("MuteNPC: Plugin loaded successfully");
        return true;
    } catch (const std::exception& e) {
        SKSE::log::critical("MuteNPC: Exception in SKSEPluginLoad: {}", e.what());
        return false;
    } catch (...) {
        SKSE::log::critical("MuteNPC: Unknown exception in SKSEPluginLoad");
        return false;
    }
}
