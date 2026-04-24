// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Particles Engine (Effekseer)
// Licença: MIT | Origem: effekseer/Effekseer
// ============================================================

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include <string>
#include "Log.hpp"

namespace Vamos {
    class ParticlesEngine {
    public:
        ParticlesEngine(int maxSprites = 8000) {
            // Inicializar renderizador 
            // EffekseerRendererGL suporta OpenGL, existindo outras versões p/ DX e Vulkan
            renderer = EffekseerRendererGL::Renderer::Create(maxSprites, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
            
            manager = Effekseer::Manager::Create(maxSprites);
            
            manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
            manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
            manager->SetRingRenderer(renderer->CreateRingRenderer());
            manager->SetTrackRenderer(renderer->CreateTrackRenderer());
            manager->SetModelRenderer(renderer->CreateModelRenderer());
            
            // Adicionar carregadores de arquivo p/ VFS no futuro
            manager->SetTextureLoader(renderer->CreateTextureLoader());
            manager->SetModelLoader(renderer->CreateModelLoader());
            manager->SetMaterialLoader(renderer->CreateMaterialLoader());
            
            VAMOS_CORE_INFO("[ParticlesEngine] Effekseer Inicializado (Limite: {0} sprites)", maxSprites);
        }

        ~ParticlesEngine() {
            manager.Reset();
            renderer.Reset();
        }

        Effekseer::EffectRef LoadEffect(const std::string& path) {
            char16_t outPath[512];
            Effekseer::ConvertUtf8ToUtf16(outPath, 512, path.c_str());
            auto effect = Effekseer::Effect::Create(manager, outPath);
            if (!effect) {
                VAMOS_CORE_ERROR("[ParticlesEngine] Falha ao carregar {0}", path);
            }
            return effect;
        }

        Effekseer::Handle Play(Effekseer::EffectRef effect, float x, float y, float z) {
            if (!effect) return -1;
            return manager->Play(effect, x, y, z);
        }

        void Update(float deltaFrames = 1.0f) {
            manager->Update(deltaFrames);
        }

        void Render(const float* projectionMatrix, const float* cameraMatrix) {
            // Effekseer espera matrizes de 16 elementos float
            ::Effekseer::Matrix44 proj, cam;
            memcpy(proj.Values, projectionMatrix, sizeof(float) * 16);
            memcpy(cam.Values, cameraMatrix, sizeof(float) * 16);

            renderer->SetProjectionMatrix(proj);
            renderer->SetCameraMatrix(cam);

            renderer->BeginRendering();
            manager->Draw();
            renderer->EndRendering();
        }

    private:
        ::Effekseer::ManagerRef manager;
        ::EffekseerRendererGL::RendererRef renderer;
    };
}
