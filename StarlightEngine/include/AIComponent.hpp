// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace starlight {

    enum class BTStatus {
        SUCCESS,
        FAILURE,
        RUNNING
    };

    // Forward declaration of Entity for target-specific logic if needed
    using AIActionFn = std::function<BTStatus(entt::entity)>;

    struct BTNode {
        virtual ~BTNode() = default;
        virtual BTStatus Tick(entt::entity entity) = 0;
    };

    struct BTAction : public BTNode {
        AIActionFn action;
        BTAction(AIActionFn fn) : action(fn) {}
        BTStatus Tick(entt::entity entity) override { return action(entity); }
    };

    struct BTComposite : public BTNode {
        std::vector<std::unique_ptr<BTNode>> children;
        void AddChild(std::unique_ptr<BTNode> child) { children.push_back(std::move(child)); }
    };

    struct BTSelector : public BTComposite {
        BTStatus Tick(entt::entity entity) override {
            for (auto& child : children) {
                BTStatus status = child->Tick(entity);
                if (status != BTStatus::FAILURE) return status;
            }
            return BTStatus::FAILURE;
        }
    };

    struct BTSequence : public BTComposite {
        BTStatus Tick(entt::entity entity) override {
            for (auto& child : children) {
                BTStatus status = child->Tick(entity);
                if (status != BTStatus::SUCCESS) return status;
            }
            return BTStatus::SUCCESS;
        }
    };

    struct BTMoveTo : public BTNode {
        glm::vec3 target;
        std::vector<glm::vec3> path;
        bool pathFound = false;

        BTMoveTo(const glm::vec3& t) : target(t) {}
        BTStatus Tick(entt::entity entity) override;
    };

    struct AIComponent {
        std::unique_ptr<BTNode> root;
        std::string currentState = "Idle";
        
        // Simple blackboard for AI data
        float timer = 0.0f;
        glm::vec3 targetPos = {0,0,0};
    };

}
