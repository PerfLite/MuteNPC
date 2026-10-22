#pragma once
#include <string>

namespace MuteNPC
{
    struct I18nStrings
    {
        std::string menu_section{ "MuteNPC/Settings" };
        std::string menu_header{ "=== MuteNPC: Silence NPCs ===" };
        std::string menu_enable{ "Enable mod" };
        std::string target_section{ "Crosshair Target:" };
        std::string target_name{ "  Target: %s" };
        std::string target_none{ "  (aim at any NPC)" };
        std::string target_btn_mute{ "Mute this target" };
        std::string target_btn_unmute{ "Unmute this target" };
        std::string params_section{ "Parameters:" };
        std::string params_hide_subtitles{ "Hide subtitles for muted dialogue" };
        std::string list_header{ "Muted NPC List (%zu)" };
        std::string list_empty{ "List is empty. Aim at an NPC and click \"Mute this target\"." };
        std::string list_btn_delete{ "Remove" };
        std::string list_btn_clear{ "Clear entire list" };
        std::string settings_btn_reset{ "Reset settings to default" };
    };

    class I18n
    {
    public:
        static I18n& GetSingleton();

        void Load();
        const I18nStrings& Strings() const { return m_strings; }

    private:
        I18n() = default;
        ~I18n() = default;

        std::string DetectLanguageSuffix();
        void LoadFromFile(const std::filesystem::path& a_path);
        void SetRussianDefaults();

        I18nStrings m_strings;
    };

    inline const I18nStrings& Tr()
    {
        return I18n::GetSingleton().Strings();
    }
}
