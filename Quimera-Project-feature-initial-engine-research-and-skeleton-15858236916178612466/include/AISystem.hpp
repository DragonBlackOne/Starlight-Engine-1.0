// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include "AIComponent.hpp"
#include <iostream>

namespace Vamos {
    class AISystem {
    public:
        static void Update(entt::registry& registry, float dt) {
            auto view = registry.view<AIComponent>();
            static BT::BehaviorTreeFactory factory;
            static bool registered = false;

            if (!registered) {
                // Registrar nós para o Titan Showcase
                factory.registerSimpleCondition("IsCoreStable", [](BT::TreeNode&) { return BT::NodeStatus::SUCCESS; });
                factory.registerSimpleAction("AlertSentinels", [](BT::TreeNode&) { return BT::NodeStatus::SUCCESS; });
                factory.registerSimpleAction("MoveToNextWaypoint", [](BT::TreeNode&) { return BT::NodeStatus::SUCCESS; });
                factory.registerSimpleAction("ScanArea", [](BT::TreeNode&) { return BT::NodeStatus::SUCCESS; });
                registered = true;
            }

            for (auto entity : view) {
                auto& ai = registry.get<AIComponent>(entity);
                
                if (!ai.initialized && !ai.treePath.empty()) {
                    try {
                        ai.tree = factory.createTreeFromFile(ai.treePath, ai.blackboard);
                        ai.initialized = true;
                        std::cout << "[AI] Tree loaded for entity " << (uint32_t)entity << ": " << ai.treePath << std::endl;
                    } catch (const std::exception& e) {
                        // Só loga uma vez para não inundar o console
                        std::cerr << "[AI] Error loading tree " << ai.treePath << ": " << e.what() << std::endl;
                        ai.initialized = true; // Marca como 'tentado' para parar de tentar carregar
                        ai.tree = BT::Tree(); // Null tree
                    }
                }

                if (ai.initialized && ai.tree.rootNode()) {
                    ai.tree.tickOnce();
                }
            }
        }
    };
}
