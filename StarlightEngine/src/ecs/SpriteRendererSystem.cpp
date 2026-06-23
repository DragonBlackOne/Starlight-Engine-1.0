#include "SpriteRendererSystem.hpp"
#include "Renderer2D.hpp"
#include "Engine.hpp"
#include "Log.hpp"

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

        Renderer2D::BeginBatch();

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& sprite = view.get<SpriteComponent>(entity);

            if (!sprite.visible) continue;

            glm::vec2 pos = {transform.position.x, transform.position.y};
            glm::vec2 size = {transform.scale.x, transform.scale.y};

            Renderer2D::DrawQuad(pos, size, sprite.textureID, sprite.color, sprite.layer);
        }

        Renderer2D::EndBatch();
    }

}
