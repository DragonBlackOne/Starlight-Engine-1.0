#include "CVarSystem.hpp"
#include "Log.hpp"
#include <algorithm>
#include <sstream>

namespace starlight {

CVarSystem::CVarSystem() {}

CVarSystem::~CVarSystem() {
    OnShutdown();
}

bool CVarSystem::OnInitialize(const EngineContext& context) {
    (void)context;
    Log::Info("CVarSystem: Initialized.");
    return true;
}

void CVarSystem::OnShutdown() {
    m_cvars.clear();
    m_changeCallbacksInt.clear();
    m_changeCallbacksFloat.clear();
    m_changeCallbacksBool.clear();
    m_changeCallbacksString.clear();
}

void CVarSystem::RegisterInt(const std::string& name, int defaultValue, const std::string& description, std::function<void(int)> onChange) {
    if (Exists(name)) return;
    CVar cv;
    cv.name = name;
    cv.description = description;
    cv.type = CVarType::Int;
    cv.value = defaultValue;
    m_cvars[name] = cv;
    if (onChange) {
        m_changeCallbacksInt[name] = onChange;
        onChange(defaultValue);
    }
}

void CVarSystem::RegisterFloat(const std::string& name, float defaultValue, const std::string& description, std::function<void(float)> onChange) {
    if (Exists(name)) return;
    CVar cv;
    cv.name = name;
    cv.description = description;
    cv.type = CVarType::Float;
    cv.value = defaultValue;
    m_cvars[name] = cv;
    if (onChange) {
        m_changeCallbacksFloat[name] = onChange;
        onChange(defaultValue);
    }
}

void CVarSystem::RegisterBool(const std::string& name, bool defaultValue, const std::string& description, std::function<void(bool)> onChange) {
    if (Exists(name)) return;
    CVar cv;
    cv.name = name;
    cv.description = description;
    cv.type = CVarType::Bool;
    cv.value = defaultValue;
    m_cvars[name] = cv;
    if (onChange) {
        m_changeCallbacksBool[name] = onChange;
        onChange(defaultValue);
    }
}

void CVarSystem::RegisterString(const std::string& name, const std::string& defaultValue, const std::string& description, std::function<void(const std::string&)> onChange) {
    if (Exists(name)) return;
    CVar cv;
    cv.name = name;
    cv.description = description;
    cv.type = CVarType::String;
    cv.value = defaultValue;
    m_cvars[name] = cv;
    if (onChange) {
        m_changeCallbacksString[name] = onChange;
        onChange(defaultValue);
    }
}

int CVarSystem::GetInt(const std::string& name) const {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::Int) {
        return std::get<int>(it->second.value);
    }
    return 0;
}

float CVarSystem::GetFloat(const std::string& name) const {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::Float) {
        return std::get<float>(it->second.value);
    }
    return 0.0f;
}

bool CVarSystem::GetBool(const std::string& name) const {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::Bool) {
        return std::get<bool>(it->second.value);
    }
    return false;
}

std::string CVarSystem::GetString(const std::string& name) const {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::String) {
        return std::get<std::string>(it->second.value);
    }
    return "";
}

void CVarSystem::SetInt(const std::string& name, int value) {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::Int) {
        it->second.value = value;
        auto cbIt = m_changeCallbacksInt.find(name);
        if (cbIt != m_changeCallbacksInt.end()) {
            cbIt->second(value);
        }
    }
}

void CVarSystem::SetFloat(const std::string& name, float value) {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::Float) {
        it->second.value = value;
        auto cbIt = m_changeCallbacksFloat.find(name);
        if (cbIt != m_changeCallbacksFloat.end()) {
            cbIt->second(value);
        }
    }
}

void CVarSystem::SetBool(const std::string& name, bool value) {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::Bool) {
        it->second.value = value;
        auto cbIt = m_changeCallbacksBool.find(name);
        if (cbIt != m_changeCallbacksBool.end()) {
            cbIt->second(value);
        }
    }
}

void CVarSystem::SetString(const std::string& name, const std::string& value) {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end() && it->second.type == CVarType::String) {
        it->second.value = value;
        auto cbIt = m_changeCallbacksString.find(name);
        if (cbIt != m_changeCallbacksString.end()) {
            cbIt->second(value);
        }
    }
}

bool CVarSystem::SetFromString(const std::string& name, const std::string& valStr) {
    auto it = m_cvars.find(name);
    if (it == m_cvars.end()) return false;

    try {
        if (it->second.type == CVarType::Int) {
            SetInt(name, std::stoi(valStr));
            return true;
        } else if (it->second.type == CVarType::Float) {
            SetFloat(name, std::stof(valStr));
            return true;
        } else if (it->second.type == CVarType::Bool) {
            std::string lower = valStr;
            std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            bool bVal = (lower == "true" || lower == "1" || lower == "on" || lower == "yes");
            SetBool(name, bVal);
            return true;
        } else if (it->second.type == CVarType::String) {
            SetString(name, valStr);
            return true;
        }
    } catch (...) {
        Log::Warn("CVarSystem: Failed to parse '{}' for CVar '{}'", valStr, name);
    }
    return false;
}

bool CVarSystem::Exists(const std::string& name) const {
    return m_cvars.find(name) != m_cvars.end();
}

CVarType CVarSystem::GetType(const std::string& name) const {
    auto it = m_cvars.find(name);
    if (it != m_cvars.end()) {
        return it->second.type;
    }
    return CVarType::Int;
}

std::vector<CVar> CVarSystem::GetCVars() const {
    std::vector<CVar> list;
    for (const auto& pair : m_cvars) {
        list.push_back(pair.second);
    }
    return list;
}

} // namespace starlight
