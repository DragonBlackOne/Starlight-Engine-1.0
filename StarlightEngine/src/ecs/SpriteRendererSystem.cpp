#include "SpriteRendererSystem.hpp"
#include "Renderer2D.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include "CVarSystem.hpp"
#include <cmath>
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

namespace starlight {

    bool SpriteRendererSystem::OnInitialize(const EngineContext& /*context*/) {
        Log::Info("SpriteRendererSystem: Initialized.");
        return true;
    }

    void SpriteRendererSystem::OnShutdown() {
        Log::Info("SpriteRendererSystem: Shutdown.");
    }

    void SpriteRendererSystem::OnRender() {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;

        auto& registry = activeScene->GetRegistry();
        auto view = registry.view<TransformComponent, SpriteComponent>();

        bool cullEnabled = false;
        float c_left = 0.0f, c_right = 0.0f, c_bottom = 0.0f, c_top = 0.0f;

        auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
        bool enableCulling = cvarSys ? cvarSys->GetBool("r_cull2d") : true;

        if (enableCulling) {
            auto cameraView = registry.view<TransformComponent, Camera2DComponent>();
            for (auto camEnt : cameraView) {
                auto& cam = cameraView.get<Camera2DComponent>(camEnt);
                if (cam.primary) {
                    auto& camTransform = cameraView.get<TransformComponent>(camEnt);
                    float width = (float)Engine::Get().GetWindow().GetWidth();
                    float height = (float)Engine::Get().GetWindow().GetHeight();
                    float halfW = width * 0.5f / cam.zoom;
                    float halfH = height * 0.5f / cam.zoom;

                    if (cam.rotation != 0.0f) {
                        float cosA = std::abs(std::cos(cam.rotation));
                        float sinA = std::abs(std::sin(cam.rotation));
                        float rotatedHalfW = halfW * cosA + halfH * sinA;
                        float rotatedHalfH = halfW * sinA + halfH * cosA;
                        c_left = camTransform.position.x - rotatedHalfW;
                        c_right = camTransform.position.x + rotatedHalfW;
                        c_bottom = camTransform.position.y - rotatedHalfH;
                        c_top = camTransform.position.y + rotatedHalfH;
                    } else {
                        c_left = camTransform.position.x - halfW;
                        c_right = camTransform.position.x + halfW;
                        c_bottom = camTransform.position.y - halfH;
                        c_top = camTransform.position.y + halfH;
                    }
                    cullEnabled = true;
                    break;
                }
            }
        }

        Renderer2D::BeginBatch();

        uint32_t totalSprites = 0;
        uint32_t visibleSprites = 0;

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& sprite = view.get<SpriteComponent>(entity);

            if (!sprite.visible) continue;

            totalSprites++;

            glm::vec2 pos = {transform.position.x, transform.position.y};
            glm::vec2 size = {transform.scale.x, transform.scale.y};
            float rotation = glm::eulerAngles(transform.rotation).z;

            // Calcular os cantos rotacionados do sprite usando o pivô para culling correto
            glm::vec2 center = {pos.x + size.x * sprite.pivot.x, pos.y + size.y * sprite.pivot.y};
            float cosA = std::cos(rotation);
            float sinA = std::sin(rotation);

            glm::vec2 local[4] = {
                {-size.x * sprite.pivot.x, -size.y * sprite.pivot.y},
                {size.x * (1.0f - sprite.pivot.x), -size.y * sprite.pivot.y},
                {size.x * (1.0f - sprite.pivot.x), size.y * (1.0f - sprite.pivot.y)},
                {-size.x * sprite.pivot.x, size.y * (1.0f - sprite.pivot.y)}
            };

            glm::vec2 positions[4];
            for (int i = 0; i < 4; i++) {
                positions[i] = {
                    center.x + local[i].x * cosA - local[i].y * sinA,
                    center.y + local[i].x * sinA + local[i].y * cosA
                };
            }

            if (cullEnabled) {
                float s_left = positions[0].x;
                float s_right = positions[0].x;
                float s_bottom = positions[0].y;
                float s_top = positions[0].y;
                for (int i = 1; i < 4; i++) {
                    s_left = std::min(s_left, positions[i].x);
                    s_right = std::max(s_right, positions[i].x);
                    s_bottom = std::min(s_bottom, positions[i].y);
                    s_top = std::max(s_top, positions[i].y);
                }

                if (s_right < c_left || s_left > c_right || s_top < c_bottom || s_bottom > c_top) {
                    continue; // Culled!
                }
            }

            visibleSprites++;

            Renderer2D::DrawQuad(pos, size, rotation, sprite.pivot, sprite.textureID, sprite.color, sprite.layer, sprite.flipX, sprite.flipY);
        }

        Renderer2D::Increment2DStats(totalSprites, visibleSprites);
        Renderer2D::EndBatch();
    }

}
