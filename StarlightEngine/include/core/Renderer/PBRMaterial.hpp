#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <string>
#include <cmath>

namespace starlight::renderer {

struct PBRMaterial {
    glm::vec3 albedo{ 1.0f, 1.0f, 1.0f };
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    glm::vec3 emission{ 0.0f, 0.0f, 0.0f };
    float normalScale = 1.0f;

    // Subsurface Scattering (Skin / Wax)
    bool isSkin = false;
    glm::vec3 subsurfaceColor{ 0.92f, 0.38f, 0.25f };
    float subsurfaceFactor = 0.8f;

    // Texture Handles (0 = none)
    uint32_t albedoMap = 0;
    uint32_t normalMap = 0;
    uint32_t metallicMap = 0;
    uint32_t roughnessMap = 0;
    uint32_t aoMap = 0;
    uint32_t emissionMap = 0;

    std::string name = "PBR_Material";

    // -----------------------------------------------------------------------
    // Cook-Torrance BRDF Helper Calculations
    // -----------------------------------------------------------------------

    /**
     * @brief Normal Distribution Function (Trowbridge-Reitz GGX)
     */
    static float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float r) {
        float a = r * r;
        float a2 = a * a;
        float NdotH = std::max(glm::dot(N, H), 0.0f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
        denom = glm::pi<float>() * denom * denom;

        return denom > 0.000001f ? nom / denom : 1.0f;
    }

    /**
     * @brief Geometry Function (Schlick-GGX)
     */
    static float GeometrySchlickGGX(float NdotV, float r) {
        float k = (r + 1.0f) * (r + 1.0f) / 8.0f;
        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;
        return denom > 0.000001f ? nom / denom : 1.0f;
    }

    /**
     * @brief Smith Geometric Shadowing Function
     */
    static float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float r) {
        float NdotV = std::max(glm::dot(N, V), 0.0f);
        float NdotL = std::max(glm::dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, r);
        float ggx1 = GeometrySchlickGGX(NdotL, r);
        return ggx1 * ggx2;
    }

    /**
     * @brief Fresnel-Schlick Approximation
     */
    static glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0) {
        return F0 + (glm::vec3(1.0f) - F0) * std::pow(std::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }
};

} // namespace starlight::renderer
