// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <filesystem>

namespace Vamos {
    struct ScriptComponent {
        std::string scriptPath;
        std::filesystem::file_time_type lastModified;

        ScriptComponent(std::string path = "") : scriptPath(path) {
            if (!path.empty() && std::filesystem::exists(path)) {
                lastModified = std::filesystem::last_write_time(path);
            }
        }
    };
}
