#pragma once
#include <vector>
#include <memory>
#include "Shader.hpp"
#include "Mesh.hpp"

namespace starlight {

    class PostProcessing {
    public:
        static void Initialize();
        static void Shutdown();

        // Bloom: Dual-filtering downsample/upsample chain
        static void RenderBloom(uint32_t inputTexture, uint32_t width, uint32_t height);
        
        // Tonemapping & Color Grading
        static void RenderFinalComposition(uint32_t sceneTex, uint32_t bloomTex, float exposure, float gamma);

        static uint32_t GetBloomTexture();

    private:
        struct BloomMip {
            uint32_t texture;
            glm::ivec2 size;
        };

        static std::vector<BloomMip> s_bloomMips;
        static uint32_t s_bloomFBO;
        static std::shared_ptr<Shader> s_downsampleShader;
        static std::shared_ptr<Shader> s_upsampleShader;
        static std::shared_ptr<Shader> s_compositionShader;
        static std::shared_ptr<Mesh> s_quadMesh;
    };

}
