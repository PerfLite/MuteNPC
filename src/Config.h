#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>

namespace MuteNPC
{
    struct MutedActorInfo
    {
        uint32_t formID{ 0 };      // Ref FormID (0 = не сравнивать по рефу)
        uint32_t baseFormID{ 0 };  // NPC Base FormID (0 = не сравнивать по базе)
        std::string plugin;        // Плагин рефа
        std::string basePlugin;    // Плагин базы
        std::string name;          // Display name
    };

    struct Settings
    {
        bool enabled{ true };
        bool muteVoiceAudio{ true };
        bool blockLipSync{ false };
        bool blockSubtitles{ true };
        std::vector<MutedActorInfo> mutedList;
    };

    class Config
    {
    public:
        static Config& GetSingleton();

        void Load();
        void Save();

        Settings& GetSettings() { return m_settings; }
        const Settings& GetSettings() const { return m_settings; }

        bool IsMuted(RE::Actor* a_actor) const;
        bool MuteActor(RE::Actor* a_actor);
        bool UnmuteActor(RE::Actor* a_actor);
        bool RemoveAt(size_t a_index);

    private:
        Config() = default;
        ~Config() = default;

        Settings m_settings;
        mutable std::recursive_mutex m_mutex;
        std::filesystem::path m_configPath;
    };
}
