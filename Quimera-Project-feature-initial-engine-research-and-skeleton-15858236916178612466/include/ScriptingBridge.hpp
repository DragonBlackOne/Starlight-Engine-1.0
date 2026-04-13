#pragma once
// ============================================================
// Vamos Engine - LUA Scripting Bridge
// Ponte Universal para amarrar os componentes AAA do C++ ao Lua
// ============================================================

#include <sol/sol.hpp>
#include <string>
#include <memory>
#include "Log.hpp"
#include "FileSystem.hpp"
#include "Particles.hpp"

// ---- Sem forward declarations, assumimos os tipos em runtime ou Engine.hpp
namespace Vamos {
    class LuaBridge {
    public:
        // Inicializa todas as dependências do Engine no roteiro Lua
        static void BindAll(sol::state& lua, ParticlesEngine* particlesSystem) {
            // Namespace interno
            sol::table vamosTable = lua.create_named_table("Vamos");

            BindMath(lua);
            BindLog(lua, vamosTable);
            BindFileSystem(lua, vamosTable);
            BindParticles(lua, vamosTable, particlesSystem);

            VAMOS_CORE_INFO("[ScriptingBridge] Todos os Módulos Lua Ancorados com Sucesso.");
        }

    private:
        static void BindMath(sol::state& lua) {
            // Em Engine.hpp o Vector3 já estava sendo feito inline. Vamos reaproveitá-lo lá ou abstrair.
            // Para manter C++ clean-headers, as definições pesadas ficam na Engine.hpp.
            // Porém, podemos injetar constantes!
            sol::table mathTable = lua["Vamos"]["Math"] = lua.create_table();
            mathTable["PI"] = 3.14159265f;
        }

        static void BindLog(sol::state& lua, sol::table& vamos) {
            sol::table logTable = lua.create_table();
            logTable.set_function("Info", [](const std::string& msg) { VAMOS_CORE_INFO("[Lua] {0}", msg); });
            logTable.set_function("Warn", [](const std::string& msg) { VAMOS_CORE_WARN("[Lua] {0}", msg); });
            logTable.set_function("Error", [](const std::string& msg) { VAMOS_CORE_ERROR("[Lua] {0}", msg); });
            vamos["Log"] = logTable;
        }

        static void BindFileSystem(sol::state& lua, sol::table& vamos) {
            sol::table fsTable = lua.create_table();
            fsTable.set_function("ReadText", [](const std::string& filename) { 
                return FileSystem::ReadText(filename); 
            });
            fsTable.set_function("Exists", [](const std::string& filename) { 
                return FileSystem::Exists(filename); 
            });
            vamos["FileSystem"] = fsTable;
        }

        static void BindParticles(sol::state& lua, sol::table& vamos, ParticlesEngine* particles) {
            sol::table pTable = lua.create_table();
            
            pTable.set_function("Load", [particles](const std::string& path) {
                if (particles) particles->LoadEffect(path);
                return path; 
            });

            pTable.set_function("Play", [particles](const std::string& effectId, float x, float y, float z) {
                if (particles) {
                    VAMOS_CORE_TRACE("Tocando particula do lua: {0} em ({1},{2},{3})", effectId, x, y, z);
                }
            });
            vamos["Particles"] = pTable;
        }
    };
}
