#include "Pch.h"
#include "Config.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace MuteNPC
{
    static std::string ExtractBool(const std::string& a_json, const std::string& a_key, bool a_default)
    {
        auto pos = a_json.find("\"" + a_key + "\"");
        if (pos == std::string::npos) return a_default ? "true" : "false";
        pos = a_json.find(':', pos);
        if (pos == std::string::npos) return a_default ? "true" : "false";

        auto end = a_json.find_first_of(",}\n", pos);
        std::string val = a_json.substr(pos + 1, end - pos - 1);
        return val.find("true") != std::string::npos ? "true" : "false";
    }

    static std::string ExtractString(const std::string& a_obj, const std::string& a_key)
    {
        auto kPos = a_obj.find("\"" + a_key + "\"");
        if (kPos == std::string::npos) return {};
        auto colon = a_obj.find(':', kPos);
        if (colon == std::string::npos) return {};
        auto q1 = a_obj.find('"', colon);
        if (q1 == std::string::npos) return {};
        auto q2 = a_obj.find('"', q1 + 1);
        if (q2 == std::string::npos) return {};
        return a_obj.substr(q1 + 1, q2 - q1 - 1);
    }

    static uint32_t ExtractUInt(const std::string& a_obj, const std::string& a_key)
    {
        auto kPos = a_obj.find("\"" + a_key + "\"");
        if (kPos == std::string::npos) return 0;
        auto colon = a_obj.find(':', kPos);
        if (colon == std::string::npos) return 0;
        auto end = a_obj.find_first_of(",}\n", colon);
        if (end == std::string::npos) end = a_obj.size();
        try {
            return static_cast<uint32_t>(std::stoul(a_obj.substr(colon + 1, end - colon - 1), nullptr, 0));
        } catch (...) {
            return 0;
        }
    }

    // Убираем символы, ломающие наш самодельный JSON-парсер
    static std::string SanitizeName(std::string a_name)
    {
        for (auto& c : a_name) {
            if (c == '"' || c == '\\' || c == '{' || c == '}' || c == '[' || c == ']') c = ' ';
        }
        return a_name;
    }

    // Подходит ли форма под сохранённый ID
    static bool FormMatches(uint32_t a_stored, const std::string& a_plugin, const RE::TESForm* a_form)
    {
        if (a_stored == 0 || !a_form) return false;

        if (!a_plugin.empty()) {
            const auto* file = a_form->GetFile(0);
            if (!file) return false;
            const std::string_view fn = file->GetFilename();
            return fn.size() == a_plugin.size() &&
                   _strnicmp(fn.data(), a_plugin.c_str(), fn.size()) == 0 &&
                   a_form->GetLocalFormID() == a_stored;
        }

        const uint32_t full = a_form->GetFormID();
        if (full == a_stored) return true;
        // Для записей без индекса плагина: сравниваем локальную часть FormID
        return (a_stored >> 24) == 0 && (full & 0x00FFFFFF) == a_stored;
    }

    static bool ActorMatchesEntry(const MutedActorInfo& a_item, RE::Actor* a_actor)
    {
        if (FormMatches(a_item.formID, a_item.plugin, a_actor)) return true;
        auto* base = a_actor->GetActorBase();
        return FormMatches(a_item.baseFormID, a_item.basePlugin, base);
    }

    Config& Config::GetSingleton()
    {
        static Config s_instance;
        return s_instance;
    }

    void Config::Load()
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        try {
            m_configPath = std::filesystem::current_path() / "Data" / "SKSE" / "Plugins" / "MuteNPC.json";
            if (!std::filesystem::exists(m_configPath)) {
                auto legacy = std::filesystem::current_path() / "Data" / "SKSE" / "Plugins" / "QuietNPC.json";
                if (std::filesystem::exists(legacy)) {
                    m_configPath = legacy;
                } else {
                    m_configPath = std::filesystem::path("Data") / "SKSE" / "Plugins" / "MuteNPC.json";
                }
            }

            if (!std::filesystem::exists(m_configPath)) {
                Save();
                return;
            }

            std::ifstream file(m_configPath);
            if (!file.is_open()) return;

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            file.close();

            m_settings.enabled = (ExtractBool(content, "enabled", true) == "true");
            m_settings.muteVoiceAudio = (ExtractBool(content, "muteVoiceAudio", true) == "true");
            m_settings.blockLipSync = (ExtractBool(content, "blockLipSync", false) == "true");
            m_settings.blockSubtitles = (ExtractBool(content, "blockSubtitles", true) == "true");

            // Parse mutedList
            m_settings.mutedList.clear();
            auto listPos = content.find("\"mutedList\"");
            if (listPos != std::string::npos) {
                auto arrStart = content.find('[', listPos);
                auto arrEnd = content.find(']', arrStart);
                if (arrStart != std::string::npos && arrEnd != std::string::npos) {
                    std::string listStr = content.substr(arrStart + 1, arrEnd - arrStart - 1);
                    size_t objPos = 0;
                    while ((objPos = listStr.find('{', objPos)) != std::string::npos) {
                        auto objEnd = listStr.find('}', objPos);
                        if (objEnd == std::string::npos) break;

                        std::string objStr = listStr.substr(objPos, objEnd - objPos + 1);

                        MutedActorInfo info;
                        info.formID = ExtractUInt(objStr, "formID");
                        info.baseFormID = ExtractUInt(objStr, "baseFormID");
                        info.plugin = ExtractString(objStr, "plugin");
                        info.basePlugin = ExtractString(objStr, "basePlugin");
                        info.name = ExtractString(objStr, "name");

                        if (info.formID != 0 || info.baseFormID != 0) {
                            m_settings.mutedList.push_back(info);
                        }

                        objPos = objEnd + 1;
                    }
                }
            }

            SKSE::log::info("MuteNPC: Loaded config (Muted NPCs: {})", m_settings.mutedList.size());
        } catch (const std::exception& e) {
            SKSE::log::error("MuteNPC: Failed to load config: {}", e.what());
        }
    }

    void Config::Save()
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        try {
            if (m_configPath.empty() || m_configPath.filename() == "QuietNPC.json") {
                m_configPath = std::filesystem::current_path() / "Data" / "SKSE" / "Plugins" / "MuteNPC.json";
            }

            auto parentDir = m_configPath.parent_path();
            if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
                std::filesystem::create_directories(parentDir);
            }

            std::ofstream file(m_configPath);
            if (!file.is_open()) return;

            file << "{\n";
            file << "  \"enabled\": " << (m_settings.enabled ? "true" : "false") << ",\n";
            file << "  \"muteVoiceAudio\": " << (m_settings.muteVoiceAudio ? "true" : "false") << ",\n";
            file << "  \"blockLipSync\": " << (m_settings.blockLipSync ? "true" : "false") << ",\n";
            file << "  \"blockSubtitles\": " << (m_settings.blockSubtitles ? "true" : "false") << ",\n";
            file << "  \"mutedList\": [\n";

            for (size_t i = 0; i < m_settings.mutedList.size(); ++i) {
                const auto& item = m_settings.mutedList[i];
                file << "    {\n";
                file << "      \"name\": \"" << SanitizeName(item.name) << "\",\n";
                file << "      \"plugin\": \"" << SanitizeName(item.plugin) << "\",\n";
                file << "      \"formID\": " << item.formID << ",\n";
                file << "      \"basePlugin\": \"" << SanitizeName(item.basePlugin) << "\",\n";
                file << "      \"baseFormID\": " << item.baseFormID << "\n";
                file << "    }" << (i + 1 < m_settings.mutedList.size() ? "," : "") << "\n";
            }

            file << "  ]\n";
            file << "}\n";
            file.close();

            SKSE::log::info("MuteNPC: Saved config (Muted NPCs: {})", m_settings.mutedList.size());
        } catch (const std::exception& e) {
            SKSE::log::error("MuteNPC: Failed to save config: {}", e.what());
        }
    }

    bool Config::IsMuted(RE::Actor* a_actor) const
    {
        if (!a_actor) return false;

        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        if (!m_settings.enabled) return false;

        for (const auto& item : m_settings.mutedList) {
            if (ActorMatchesEntry(item, a_actor)) return true;
        }
        return false;
    }

    bool Config::MuteActor(RE::Actor* a_actor)
    {
        if (!a_actor) return false;

        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        for (const auto& item : m_settings.mutedList) {
            if (ActorMatchesEntry(item, a_actor)) return false;   // уже заглушен
        }

        auto* base = a_actor->GetActorBase();

        MutedActorInfo info;

        // Уникальных NPC (Ульфрик, Д'Аз...) глушим по базе - это один и тот же человек.
        // Обычных (стражники, бандиты, горожане) - ТОЛЬКО по конкретному рефу,
        // иначе заглушатся все NPC с той же базой.
        const bool unique = base && base->IsUnique();

        auto fillRef = [&]() {
            if (const auto* file = a_actor->GetFile(0)) {
                info.plugin = std::string(file->GetFilename());
                info.formID = a_actor->GetLocalFormID();
            } else {
                info.formID = a_actor->GetFormID();
            }
        };
        auto fillBase = [&]() {
            if (const auto* file = base->GetFile(0)) {
                info.basePlugin = std::string(file->GetFilename());
                info.baseFormID = base->GetLocalFormID();
            } else {
                info.baseFormID = base->GetFormID();
            }
        };

        fillRef();
        if (unique && base) fillBase();

        const char* name = base ? base->GetName() : a_actor->GetName();
        info.name = (name && name[0]) ? name : "NPC";

        m_settings.mutedList.push_back(info);
        Save();
        return true;
    }

    bool Config::UnmuteActor(RE::Actor* a_actor)
    {
        if (!a_actor) return false;

        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        auto it = std::remove_if(m_settings.mutedList.begin(), m_settings.mutedList.end(),
            [a_actor](const MutedActorInfo& item) { return ActorMatchesEntry(item, a_actor); });

        if (it == m_settings.mutedList.end()) return false;

        m_settings.mutedList.erase(it, m_settings.mutedList.end());
        Save();
        return true;
    }

    bool Config::RemoveAt(size_t a_index)
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        if (a_index >= m_settings.mutedList.size()) return false;
        m_settings.mutedList.erase(m_settings.mutedList.begin() + static_cast<std::ptrdiff_t>(a_index));
        Save();
        return true;
    }
}
