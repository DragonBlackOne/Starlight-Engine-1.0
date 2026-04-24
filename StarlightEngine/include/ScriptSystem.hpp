// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <sol/sol.hpp>
#include <memory>
#include <string>

namespace starlight {
    class ScriptSystem {
    public:
        ScriptSystem();
        ~ScriptSystem();

        void Initialize();
        void ExecuteFile(const std::string& path);
        void Update(float dt);

        sol::state& GetLua() { return m_lua; }

    private:
        sol::state m_lua;
        void BindCore();
    };
}
