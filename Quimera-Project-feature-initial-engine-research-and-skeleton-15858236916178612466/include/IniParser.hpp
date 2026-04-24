// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - SimpleIni
// Licença: MIT | Origem: brofield/simpleini
// ============================================================

#include "simpleini/SimpleIni.h"
#include <string>
#include "Log.hpp"

namespace Vamos {
    class Inifile {
    public:
        Inifile() {
            ini.SetUnicode();
        }

        bool Load(const std::string& path) {
            SI_Error rc = ini.LoadFile(path.c_str());
            if (rc < 0) {
                VAMOS_CORE_ERROR("[Inifile] Falha ao carregar {0}", path);
                return false;
            }
            return true;
        }

        bool Save(const std::string& path) {
            SI_Error rc = ini.SaveFile(path.c_str());
            if (rc < 0) {
                VAMOS_CORE_ERROR("[Inifile] Falha ao salvar {0}", path);
                return false;
            }
            return true;
        }

        std::string GetValue(const char* section, const char* key, const char* def = "") {
            return ini.GetValue(section, key, def);
        }

        void SetValue(const char* section, const char* key, const char* value) {
            ini.SetValue(section, key, value);
        }

    private:
        CSimpleIniA ini;
    };
}
