#pragma once
#include <string>
#include <unordered_map>
#include "EngineSystem.hpp"

namespace starlight {

class ConfigSystem : public ISystem {
public:
    ConfigSystem();
    ~ConfigSystem();

    // ISystem
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    const char* GetName() const override { return "ConfigSystem"; }
    bool IsMainThreadOnly() const override { return true; }

    // IO Methods
    bool Load(const std::string& filepath);
    bool Save(const std::string& filepath);

    // Get values with fallback defaults
    int GetInt(const std::string& section, const std::string& key, int defaultVal = 0) const;
    float GetFloat(const std::string& section, const std::string& key, float defaultVal = 0.0f) const;
    bool GetBool(const std::string& section, const std::string& key, bool defaultVal = false) const;
    std::string GetString(const std::string& section, const std::string& key, const std::string& defaultVal = "") const;

    // Set values
    void SetInt(const std::string& section, const std::string& key, int value);
    void SetFloat(const std::string& section, const std::string& key, float value);
    void SetBool(const std::string& section, const std::string& key, bool value);
    void SetString(const std::string& section, const std::string& key, const std::string& value);

    // Helper to query loaded structure
    bool HasSection(const std::string& section) const;
    bool HasKey(const std::string& section, const std::string& key) const;

private:
    // Nested map: Section -> Key -> Value (represented as string)
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_data;
    std::string m_filepath;
};

} // namespace starlight
