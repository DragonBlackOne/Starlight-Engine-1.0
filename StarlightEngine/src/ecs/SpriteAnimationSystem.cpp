#include "SpriteAnimationSystem.hpp"
#include "SpriteAnimationComponent.hpp"
#include "Components.hpp"
#include "Engine.hpp"
#include "Log.hpp"

namespace starlight {
    bool SpriteAnimationSystem::OnInitialize(const EngineContext& /*context*/) {
        Log::Info("SpriteAnimationSystem: Initialized.");
        return true;
    }

    void SpriteAnimationSystem::OnShutdown() {
        Log::Info("SpriteAnimationSystem: Shutdown.");
    }

    void SpriteAnimationSystem::OnUpdate(float dt) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;
        auto& registry = activeScene->GetRegistry();

        auto view = registry.view<SpriteAnimationComponent, SpriteComponent>();
        for (auto entity : view) {
            auto& anim = view.get<SpriteAnimationComponent>(entity);
            auto& sprite = view.get<SpriteComponent>(entity);

            if (!anim.playing || anim.frames.empty() || anim.fps <= 0.0f) continue;

            anim.timer += dt;
            float frameDuration = 1.0f / anim.fps;

            while (anim.timer >= frameDuration) {
                anim.timer -= frameDuration;
                anim.currentFrame++;

                if (anim.currentFrame >= static_cast<int>(anim.frames.size())) {
                    if (anim.looping) {
                        anim.currentFrame = 0;
                    } else {
                        anim.currentFrame = static_cast<int>(anim.frames.size()) - 1;
                        anim.playing = false;
                        break;
                    }
                }
            }

            sprite.textureID = anim.frames[anim.currentFrame];
        }
    }

    void SpriteAnimationSystem::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
        (void)reads;
        writes.push_back(typeid(SpriteAnimationComponent));
        writes.push_back(typeid(SpriteComponent));
    }
}
