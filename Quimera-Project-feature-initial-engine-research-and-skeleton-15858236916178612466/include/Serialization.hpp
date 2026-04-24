// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Serialization Module
// JSON: nlohmann/json (MIT) | Binário: cereal (BSD-3)
// ============================================================

#include <nlohmann/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <fstream>
#include <string>
#include "Log.hpp"

namespace Vamos {
    using json = nlohmann::json;

    class Serializer {
    public:
        // ---- JSON ----
        static json LoadJSON(const std::string& path) {
            std::ifstream file(path);
            if (!file.is_open()) {
                VAMOS_CORE_ERROR("Serializer: Não foi possível abrir {0}", path);
                return json();
            }
            json j;
            file >> j;
            return j;
        }

        static bool SaveJSON(const std::string& path, const json& data, int indent = 4) {
            std::ofstream file(path);
            if (!file.is_open()) {
                VAMOS_CORE_ERROR("Serializer: Não foi possível salvar {0}", path);
                return false;
            }
            file << data.dump(indent);
            return true;
        }

        // ---- Cereal (Binário de alta performance) ----
        template<typename T>
        static bool SaveBinary(const std::string& path, const T& object) {
            std::ofstream file(path, std::ios::binary);
            if (!file.is_open()) return false;
            cereal::BinaryOutputArchive archive(file);
            archive(object);
            return true;
        }

        template<typename T>
        static bool LoadBinary(const std::string& path, T& object) {
            std::ifstream file(path, std::ios::binary);
            if (!file.is_open()) return false;
            cereal::BinaryInputArchive archive(file);
            archive(object);
            return true;
        }

        // ---- Cereal JSON (para debug/inspeção) ----
        template<typename T>
        static bool SaveCerealJSON(const std::string& path, const T& object) {
            std::ofstream file(path);
            if (!file.is_open()) return false;
            cereal::JSONOutputArchive archive(file);
            archive(object);
            return true;
        }

        template<typename T>
        static bool LoadCerealJSON(const std::string& path, T& object) {
            std::ifstream file(path);
            if (!file.is_open()) return false;
            cereal::JSONInputArchive archive(file);
            archive(object);
            return true;
        }
    };

    // Exemplo de componente serializável
    struct SceneData {
        std::string name;
        std::vector<std::string> entities;

        template<class Archive>
        void serialize(Archive& ar) {
            ar(CEREAL_NVP(name), CEREAL_NVP(entities));
        }
    };
}
