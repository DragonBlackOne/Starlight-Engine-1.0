// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Vamos {
    struct MaterialComponent {
        glm::vec4 baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;

        std::string albedoPath = "";
        std::string normalPath = "";
        std::string metallicPath = "";
        std::string roughnessPath = "";
        std::string aoPath = "";

        bool hasAlbedo = false;
        bool hasNormal = false;
        bool hasMetallic = false;
        bool hasRoughness = false;
        bool hasAO = false;

        // Compatibilidade legado
        glm::vec4 color = glm::vec4(1.0f);
        bool hasTexture = false;
    };
}
