#pragma once
#include <string>
#include <variant>
#include <functional>
#include <unordered_map>
#include <vector>
#include "EngineSystem.hpp"

namespace starlight {

enum class CVarType {
    Int,
    Float,
    Bool,
    String
};

struct CVar {
    std::string name;
    std::string description;
    CVarType type;
    std::variant<int, float, bool, std::string> value;
};

class CVarSystem : public ISystem {
public:
    CVarSystem();
    ~CVarSystem();

    // ISystem
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    const char* GetName() const override { return "CVarSystem"; }
    bool IsMainThreadOnly() const override { return true; }

    // Register CVars
    void RegisterInt(const std::string& name, int defaultValue, const std::string& description, std::function<void(int)> onChange = nullptr);
    void RegisterFloat(const std::string& name, float defaultValue, const std::string& description, std::function<void(float)> onChange = nullptr);
    void RegisterBool(const std::string& name, bool defaultValue, const std::string& description, std::function<void(bool)> onChange = nullptr);
    void RegisterString(const std::string& name, const std::string& defaultValue, const std::string& description, std::function<void(const std::string&)> onChange = nullptr);

    // Get CVar values
    int GetInt(const std::string& name) const;
    float GetFloat(const std::string& name) const;
    bool GetBool(const std::string& name) const;
    std::string GetString(const std::string& name) const;

    // Set CVar values (triggers callbacks)
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetBool(const std::string& name, bool value);
    void SetString(const std::string& name, const std::string& value);

    // String-based set (useful for console parsing)
    bool SetFromString(const std::string& name, const std::string& valStr);

    bool Exists(const std::string& name) const;
    CVarType GetType(const std::string& name) const;
    std::vector<CVar> GetCVars() const;

private:
    std::unordered_map<std::string, CVar> m_cvars;
    std::unordered_map<std::string, std::function<void(int)>> m_changeCallbacksInt;
    std::unordered_map<std::string, std::function<void(float)>> m_changeCallbacksFloat;
    std::unordered_map<std::string, std::function<void(bool)>> m_changeCallbacksBool;
    std::unordered_map<std::string, std::function<void(const std::string&)>> m_changeCallbacksString;
};

} // namespace starlight
