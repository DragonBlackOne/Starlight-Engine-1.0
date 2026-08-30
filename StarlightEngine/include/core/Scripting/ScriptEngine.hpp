#pragma once
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <sol/sol.hpp>
#pragma warning(pop)
#include <string>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace starlight::scripting {

class ScriptEngine {
public:
    static ScriptEngine& Get() {
        static ScriptEngine instance;
        return instance;
    }

    void Initialize() {
        if (m_initialized) return;

        m_lua.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::math,
            sol::lib::string,
            sol::lib::table,
            sol::lib::os,
            sol::lib::debug
        );

        RegisterCoreMath();
        m_initialized = true;
    }

    void Shutdown() {
        m_lua = sol::state();
        m_initialized = false;
    }

    bool ExecuteString(const std::string& scriptCode) {
        if (!m_initialized) Initialize();

        auto result = m_lua.safe_script(scriptCode, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            m_lastError = err.what();
            return false;
        }
        return true;
    }

    template<typename T>
    void SetGlobal(const std::string& name, T&& value) {
        if (!m_initialized) Initialize();
        m_lua[name] = std::forward<T>(value);
    }

    template<typename T>
    T GetGlobal(const std::string& name) {
        if (!m_initialized) Initialize();
        return m_lua[name].get<T>();
    }

    sol::state& GetLuaState() {
        if (!m_initialized) Initialize();
        return m_lua;
    }

    const std::string& GetLastError() const { return m_lastError; }
    bool IsInitialized() const { return m_initialized; }

private:
    void RegisterCoreMath() {
        // Expose glm::vec3
        m_lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
            sol::meta_function::multiplication, [](const glm::vec3& a, float b) { return a * b; }
        );

        // Expose glm::vec2
        m_lua.new_usertype<glm::vec2>("vec2",
            sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );
    }

    sol::state m_lua;
    bool m_initialized = false;
    std::string m_lastError;
};

} // namespace starlight::scripting
