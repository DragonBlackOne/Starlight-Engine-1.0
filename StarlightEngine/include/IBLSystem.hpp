#pragma once
#include <string>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "Mesh.hpp"

namespace starlight {

    struct IBLData {
        uint32_t envCubemap = 0;
        uint32_t irradianceMap = 0;
        uint32_t prefilterMap = 0;
        uint32_t brdfLUT = 0;
    };

    /**
     * @brief IBLSystem (Phase 12)
     * Handles HDR processing for professional Image Based Lighting.
     */
    class IBLSystem {
    public:
        IBLSystem();
        ~IBLSystem();

        IBLData ProcessHDR(const std::string& hdrPath);

    private:
        std::shared_ptr<Shader> m_equiToCubeShader;
        std::shared_ptr<Shader> m_irradianceShader;
        std::shared_ptr<Shader> m_prefilterShader;
        std::shared_ptr<Shader> m_brdfShader;
        
        std::shared_ptr<Mesh> m_cubeMesh;
        std::shared_ptr<Mesh> m_quadMesh;

        void SetupShaders();
    };

}
