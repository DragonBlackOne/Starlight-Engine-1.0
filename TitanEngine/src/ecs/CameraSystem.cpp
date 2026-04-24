// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "CameraSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace titan {
    void CameraSystem::Update(entt::registry& registry, float aspectRatio) {
        auto view = registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& camera = view.get<CameraComponent>(entity);

            if (camera.primary) {
                // Calcular View Matrix
                glm::vec3 front = transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 up = transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);
                camera.view = glm::lookAt(transform.position, transform.position + front, up);

                // Calcular Projection Matrix (garantir aspect ratio correto)
                camera.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearPlane, camera.farPlane);
            }
        }
    }
}
