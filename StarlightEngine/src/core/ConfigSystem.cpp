#include "ConfigSystem.hpp"
#include "Log.hpp"
#include "VFSSystem.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace starlight {

static inline std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

ConfigSystem::ConfigSystem() {}

ConfigSystem::~ConfigSystem() {
    OnShutdown();
}

bool ConfigSystem::OnInitialize(const EngineContext& context) {
    (void)context;
    Log::Info("ConfigSystem: Initialized.");
    return true;
}

void ConfigSystem::OnShutdown() {
    m_data.clear();
}

bool ConfigSystem::Load(const std::string& filepath) {
    m_filepath = filepath;
    auto bytes = VFSSystem::Get().ReadFile(filepath);
    if (bytes.empty()) {
        Log::Warn("ConfigSystem: Failed to open config file '{}' for reading.", filepath);
        return false;
    }

    m_data.clear();
    std::string content(bytes.begin(), bytes.end());
    std::stringstream file(content);
    std::string line;
    std::string currentSection = "";

    while (std::getline(file, line)) {
        line = Trim(line);
        
        // Skip empty lines or comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        // Section header
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            currentSection = Trim(line.substr(1, line.size() - 2));
            continue;
        }

        // Key-value pairs
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = Trim(line.substr(0, eqPos));
            std::string value = Trim(line.substr(eqPos + 1));
            if (!key.empty() && !currentSection.empty()) {
                m_data[currentSection][key] = value;
            }
        }
    }

    Log::Info("ConfigSystem: Loaded config file '{}' with {} sections.", filepath, m_data.size());
    return true;
}

bool ConfigSystem::Save(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        Log::Error("ConfigSystem: Failed to open config file '{}' for writing.", filepath);
        return false;
    }

    for (const auto& sectionPair : m_data) {
        file << "[" << sectionPair.first << "]\n";
        for (const auto& keyValuePair : sectionPair.second) {
            file << keyValuePair.first << "=" << keyValuePair.second << "\n";
        }
        file << "\n"; // empty line between sections
    }

    Log::Info("ConfigSystem: Saved config file '{}'.", filepath);
    return true;
}

int ConfigSystem::GetInt(const std::string& section, const std::string& key, int defaultVal) const {
    auto secIt = m_data.find(section);
    if (secIt != m_data.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            try {
                return std::stoi(keyIt->second);
            } catch (...) {
                return defaultVal;
            }
        }
    }
    return defaultVal;
}

float ConfigSystem::GetFloat(const std::string& section, const std::string& key, float defaultVal) const {
    auto secIt = m_data.find(section);
    if (secIt != m_data.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            try {
                return std::stof(keyIt->second);
            } catch (...) {
                return defaultVal;
            }
        }
    }
    return defaultVal;
}

bool ConfigSystem::GetBool(const std::string& section, const std::string& key, bool defaultVal) const {
    auto secIt = m_data.find(section);
    if (secIt != m_data.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            std::string val = keyIt->second;
            std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return (val == "true" || val == "1" || val == "yes" || val == "on");
        }
    }
    return defaultVal;
}

std::string ConfigSystem::GetString(const std::string& section, const std::string& key, const std::string& defaultVal) const {
    auto secIt = m_data.find(section);
    if (secIt != m_data.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            return keyIt->second;
        }
    }
    return defaultVal;
}

void ConfigSystem::SetInt(const std::string& section, const std::string& key, int value) {
    m_data[section][key] = std::to_string(value);
}

void ConfigSystem::SetFloat(const std::string& section, const std::string& key, float value) {
    m_data[section][key] = std::to_string(value);
}

void ConfigSystem::SetBool(const std::string& section, const std::string& key, bool value) {
    m_data[section][key] = value ? "true" : "false";
}

void ConfigSystem::SetString(const std::string& section, const std::string& key, const std::string& value) {
    m_data[section][key] = value;
}

bool ConfigSystem::HasSection(const std::string& section) const {
    return m_data.find(section) != m_data.end();
}

bool ConfigSystem::HasKey(const std::string& section, const std::string& key) const {
    auto secIt = m_data.find(section);
    if (secIt != m_data.end()) {
        return secIt->second.find(key) != secIt->second.end();
    }
    return false;
}

} // namespace starlight
