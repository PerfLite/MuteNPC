#include "Pch.h"
#include "I18n.h"
#include "RE/I/INISettingCollection.h"
#include "RE/I/INIPrefSettingCollection.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <shlobj.h>

namespace MuteNPC
{
    static std::string ExtractJsonString(const std::string& a_json, const std::string& a_key, const std::string& a_default)
    {
        auto pos = a_json.find("\"" + a_key + "\"");
        if (pos == std::string::npos) return a_default;
        pos = a_json.find(':', pos);
        if (pos == std::string::npos) return a_default;
        auto q1 = a_json.find('"', pos);
        if (q1 == std::string::npos) return a_default;
        auto q2 = a_json.find('"', q1 + 1);
        while (q2 != std::string::npos && a_json[q2 - 1] == '\\') {
            q2 = a_json.find('"', q2 + 1);
        }
        if (q2 == std::string::npos) return a_default;

        std::string val = a_json.substr(q1 + 1, q2 - q1 - 1);
        std::string res;
        for (size_t i = 0; i < val.size(); ++i) {
            if (val[i] == '\\' && i + 1 < val.size()) {
                char next = val[i + 1];
                if (next == '"') { res += '"'; ++i; continue; }
                if (next == '\\') { res += '\\'; ++i; continue; }
                if (next == 'n') { res += '\n'; ++i; continue; }
            }
            res += val[i];
        }
        return res;
    }

    I18n& I18n::GetSingleton()
    {
        static I18n s_instance;
        return s_instance;
    }

    std::string I18n::DetectLanguageSuffix()
    {
        std::string rawLang;

        try {
            if (auto* ini = RE::INISettingCollection::GetSingleton()) {
                if (auto* s = ini->GetSetting("sLanguage:General")) {
                    const char* v = s->GetString();
                    if (v && v[0]) rawLang = v;
                }
            }

            if (rawLang.empty()) {
                if (auto* iniPref = RE::INIPrefSettingCollection::GetSingleton()) {
                    if (auto* s = iniPref->GetSetting("sLanguage:General")) {
                        const char* v = s->GetString();
                        if (v && v[0]) rawLang = v;
                    }
                }
            }
        } catch (...) {}

        if (rawLang.empty()) {
            char docPath[MAX_PATH];
            if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docPath))) {
                auto p = std::filesystem::path(docPath) / "My Games" / "Skyrim Special Edition" / "Skyrim.ini";
                char buf[64] = { 0 };
                GetPrivateProfileStringA("General", "sLanguage", "", buf, sizeof(buf), p.string().c_str());
                if (buf[0]) rawLang = buf;
            }
        }

        std::string upperLang = rawLang;
        std::transform(upperLang.begin(), upperLang.end(), upperLang.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });

        if (upperLang.find("RUS") != std::string::npos || upperLang == "RU") return "RU";
        if (upperLang.find("ENG") != std::string::npos || upperLang == "EN") return "EN";
        if (upperLang.find("GER") != std::string::npos || upperLang == "DE") return "DE";
        if (upperLang.find("FRE") != std::string::npos || upperLang == "FR") return "FR";
        if (upperLang.find("SPA") != std::string::npos || upperLang == "ES") return "ES";
        if (upperLang.find("POR") != std::string::npos || upperLang == "PT") return "PT";
        if (upperLang.find("POL") != std::string::npos || upperLang == "PL") return "PL";
        if (upperLang.find("CHI") != std::string::npos || upperLang == "ZH") return "ZH";
        if (upperLang.find("JAP") != std::string::npos || upperLang == "JA") return "JA";

        return upperLang.empty() ? "EN" : upperLang;
    }

    void I18n::SetEnglishDefaults()
    {
        m_strings.menu_section = "MuteNPC/Settings";
        m_strings.menu_header = "=== MuteNPC: Silence NPCs ===";
        m_strings.menu_enable = "Enable mod";
        m_strings.target_section = "Crosshair Target:";
        m_strings.target_name = "  Target: %s";
        m_strings.target_none = "  (aim at any NPC)";
        m_strings.target_btn_mute = "Mute this target";
        m_strings.target_btn_unmute = "Unmute this target";
        m_strings.params_section = "Parameters:";
        m_strings.params_hide_subtitles = "Hide subtitles for muted dialogue";
        m_strings.params_language = "Language / Язык:";
        m_strings.list_header = "Muted NPC List (%zu)";
        m_strings.list_empty = "List is empty. Aim at an NPC and click \"Mute this target\".";
        m_strings.list_btn_delete = "Remove";
        m_strings.list_btn_clear = "Clear entire list";
        m_strings.settings_btn_reset = "Reset settings to default";
    }

    void I18n::SetRussianDefaults()
    {
        m_strings.menu_section = "MuteNPC/Настройки";
        m_strings.menu_header = "=== MuteNPC: Заглушение NPC ===";
        m_strings.menu_enable = "Включить мод";
        m_strings.target_section = "Цель под прицелом:";
        m_strings.target_name = "  Цель: %s";
        m_strings.target_none = "  (наведите прицел на любого NPC)";
        m_strings.target_btn_mute = "Заглушить эту цель";
        m_strings.target_btn_unmute = "Вернуть голос этой цели";
        m_strings.params_section = "Параметры:";
        m_strings.params_hide_subtitles = "Скрывать субтитры заглушенных фраз";
        m_strings.params_language = "Язык / Language:";
        m_strings.list_header = "Список заглушенных NPC (%zu)";
        m_strings.list_empty = "Список пуст. Наведите прицел на NPC и нажмите «Заглушить эту цель».";
        m_strings.list_btn_delete = "Удалить";
        m_strings.list_btn_clear = "Очистить весь список";
        m_strings.settings_btn_reset = "Сбросить настройки к стандартным";
    }

    void I18n::LoadFromFile(const std::filesystem::path& a_path)
    {
        std::ifstream file(a_path);
        if (!file.is_open()) return;

        std::stringstream buf;
        buf << file.rdbuf();
        std::string json = buf.str();
        file.close();

        m_strings.menu_section = ExtractJsonString(json, "Menu.Section", m_strings.menu_section);
        m_strings.menu_header = ExtractJsonString(json, "Menu.Header", m_strings.menu_header);
        m_strings.menu_enable = ExtractJsonString(json, "Menu.Enable", m_strings.menu_enable);
        m_strings.target_section = ExtractJsonString(json, "Target.Section", m_strings.target_section);
        m_strings.target_name = ExtractJsonString(json, "Target.Name", m_strings.target_name);
        m_strings.target_none = ExtractJsonString(json, "Target.None", m_strings.target_none);
        m_strings.target_btn_mute = ExtractJsonString(json, "Target.BtnMute", m_strings.target_btn_mute);
        m_strings.target_btn_unmute = ExtractJsonString(json, "Target.BtnUnmute", m_strings.target_btn_unmute);
        m_strings.params_section = ExtractJsonString(json, "Params.Section", m_strings.params_section);
        m_strings.params_hide_subtitles = ExtractJsonString(json, "Params.HideSubtitles", m_strings.params_hide_subtitles);
        m_strings.params_language = ExtractJsonString(json, "Params.Language", m_strings.params_language);
        m_strings.list_header = ExtractJsonString(json, "List.Header", m_strings.list_header);
        m_strings.list_empty = ExtractJsonString(json, "List.Empty", m_strings.list_empty);
        m_strings.list_btn_delete = ExtractJsonString(json, "List.BtnDelete", m_strings.list_btn_delete);
        m_strings.list_btn_clear = ExtractJsonString(json, "List.BtnClear", m_strings.list_btn_clear);
        m_strings.settings_btn_reset = ExtractJsonString(json, "Settings.BtnReset", m_strings.settings_btn_reset);

        SKSE::log::info("MuteNPC: Loaded translation from {}", a_path.filename().string());
    }

    void I18n::Load(std::string_view a_override)
    {
        std::string suffix;
        if (!a_override.empty() && a_override != "auto") {
            suffix = a_override;
            std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char c) {
                return static_cast<char>(std::toupper(c));
            });
        } else {
            suffix = DetectLanguageSuffix();
        }

        m_currentSuffix = suffix;
        SKSE::log::info("MuteNPC: Using language: {}", suffix);

        if (suffix == "RU") {
            SetRussianDefaults();
        } else {
            SetEnglishDefaults();
        }

        auto pluginsDir = std::filesystem::current_path() / "Data" / "SKSE" / "Plugins";
        auto langFile = pluginsDir / ("MuteNPCStrings_" + suffix + ".json");

        if (std::filesystem::exists(langFile)) {
            LoadFromFile(langFile);
            return;
        }

        auto defaultFile = pluginsDir / "MuteNPCStrings.json";
        if (std::filesystem::exists(defaultFile)) {
            LoadFromFile(defaultFile);
            return;
        }

        auto enFile = pluginsDir / "MuteNPCStrings_EN.json";
        if (std::filesystem::exists(enFile)) {
            LoadFromFile(enFile);
            return;
        }

        SKSE::log::info("MuteNPC: Using built-in defaults ({})", suffix == "RU" ? "Russian" : "English");
    }
}
