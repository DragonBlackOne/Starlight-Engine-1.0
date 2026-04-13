#pragma once
// ============================================================
// Vamos Engine - Configuration Module (yaml-cpp)
// Licença: MIT
// ============================================================

#include <yaml-cpp/yaml.h>
#include <string>
#include <fstream>
#include "Log.hpp"

namespace Vamos {
    class Config {
    public:
        static YAML::Node Load(const std::string& path) {
            try {
                YAML::Node node = YAML::LoadFile(path);
                VAMOS_CORE_INFO("Config: Carregado {0}", path);
                return node;
            } catch (const std::exception& e) {
                VAMOS_CORE_ERROR("Config: Erro ao carregar {0}: {1}", path, e.what());
                return YAML::Node();
            }
        }

        static bool Save(const std::string& path, const YAML::Node& node) {
            try {
                std::ofstream file(path);
                if (!file.is_open()) return false;
                file << node;
                return true;
            } catch (const std::exception& e) {
                VAMOS_CORE_ERROR("Config: Erro ao salvar {0}: {1}", path, e.what());
                return false;
            }
        }

        // Helpers tipados com valores padrão
        template<typename T>
        static T Get(const YAML::Node& node, const std::string& key, const T& defaultValue) {
            if (node[key]) {
                try { return node[key].as<T>(); }
                catch (...) { return defaultValue; }
            }
            return defaultValue;
        }
    };
}
