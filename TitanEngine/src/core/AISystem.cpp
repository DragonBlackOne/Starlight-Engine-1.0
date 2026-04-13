#include "AISystem.hpp"
#include "AIComponent.hpp"
#include "Log.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#include "NavigationSystem.hpp"

namespace titan {

    void AISystem::Update(entt::registry& registry, float dt) {
        auto view = registry.view<AIComponent>();
        
        for (auto entity : view) {
            auto& ai = view.get<AIComponent>(entity);
            
            // Update internal blackboard timers
            ai.timer += dt;
            
            if (ai.root) {
                // Tick the Behavior Tree
                ai.root->Tick(entity);
            }
        }
    }

    BTStatus BTMoveTo::Tick(entt::entity entity) {
        auto& registry = Engine::Get().GetSceneStack().Active()->GetRegistry();
        auto& transform = registry.get<TransformComponent>(entity);

        if (!pathFound) {
            pathFound = Engine::Get().GetNav().FindPath(transform.position, target, path);
            if (!pathFound) return BTStatus::FAILURE;
        }

        if (path.empty()) return BTStatus::SUCCESS;

        glm::vec3 nextPos = path[0];
        glm::vec3 dir = nextPos - transform.position;
        float dist = glm::length(dir);

        if (dist < 0.2f) {
            path.erase(path.begin());
            if (path.empty()) return BTStatus::SUCCESS;
        }

        transform.position += glm::normalize(dir) * 5.0f * Engine::Get().GetTime().deltaTime;
        return BTStatus::RUNNING;
    }

}
