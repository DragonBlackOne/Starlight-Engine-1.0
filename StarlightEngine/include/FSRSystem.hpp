#pragma once
#include <memory>
#include <string>
#include "GLResource.hpp"
#include "Shader.hpp"

namespace starlight {

    enum class FSRQualityMode {
        Native,
        Quality,
        Balanced,
        Performance,
        UltraPerformance
    };

    struct FSRSettings {
        bool enabled = false;
        FSRQualityMode qualityMode = FSRQualityMode::Quality;
        float sharpness = 0.8f;
    };

    // Mocks AMD FSR 2.2 / 3.0 C++ SDK structures for runtime compilation
    struct FfxFsrContextDescription {
        uint32_t flags;
        uint32_t maxRenderSizeWidth;
        uint32_t maxRenderSizeHeight;
        uint32_t displaySizeWidth;
        uint32_t displaySizeHeight;
    };

    struct FfxFsrContext {
        void* internalContext = nullptr;
    };

    class FSRSystem {
    public:
        FSRSystem();
        ~FSRSystem();

        bool Initialize(const FfxFsrContextDescription& desc);
        void Shutdown();

        void Dispatch(uint32_t inputTexture, uint32_t outputTexture, float dt, float sharpness);
        void Dispatch(uint32_t inputTexture, uint32_t outputFBO, int inputW, int inputH, int outputW, int outputH, float sharpness);

        const FSRSettings& GetSettings() const { return m_settings; }
        void SetSettings(const FSRSettings& settings) { m_settings = settings; }

    private:
        FfxFsrContext m_context;
        FfxFsrContextDescription m_desc{};
        bool m_initialized = false;
        FSRSettings m_settings;

        unsigned int m_quadVAO = 0;
        unsigned int m_quadVBO = 0;
        unsigned int m_quadEBO = 0;
        unsigned int m_intermediateFBO = 0;
        unsigned int m_intermediateTex = 0;
        std::shared_ptr<Shader> m_easuShader;
        std::shared_ptr<Shader> m_rcasShader;
    };

}
