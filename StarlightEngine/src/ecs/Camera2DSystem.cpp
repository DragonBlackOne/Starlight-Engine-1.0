#include "Camera2DSystem.hpp"
#include "Components.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Log.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace starlight {

    bool Camera2DSystem::OnInitialize(const EngineContext& /*context*/) {
        Log::Info("Camera2DSystem: Initialized.");
        return true;
    }

    void Camera2DSystem::OnShutdown() {
        Log::Info("Camera2DSystem: Shutdown.");
    }

    void Camera2DSystem::OnUpdate(float /*dt*/) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;

        auto& registry = activeScene->GetRegistry();
        auto view = registry.view<TransformComponent, Camera2DComponent>();

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& cam2D = view.get<Camera2DComponent>(entity);

            if (!cam2D.primary) continue;

            float width = (float)Engine::Get().GetWindow().GetWidth();
            float height = (float)Engine::Get().GetWindow().GetHeight();

            auto renderer = Engine::Get().GetSystem<Renderer>();
            if (renderer) {
                float halfW = width * 0.5f / cam2D.zoom;
                float halfH = height * 0.5f / cam2D.zoom;
                glm::mat4 projection = glm::ortho(
                    -halfW, halfW,
                    -halfH, halfH,
                    -1.0f, 1.0f
                );

                glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), -transform.position);
                glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), cam2D.rotation, glm::vec3(0, 0, 1));
                viewMat = rotationMat * viewMat;

                renderer->SetViewMatrix(viewMat);
                renderer->SetProjectionMatrix(projection);
            }
        }
    }

}
