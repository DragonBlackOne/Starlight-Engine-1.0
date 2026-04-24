// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <behaviortree_cpp/bt_factory.h>

namespace Vamos {
    struct AIComponent {
        std::string treePath;
        BT::Tree tree;
        bool initialized = false;
        
        // Blackboard para troca de dados com o mundo/Lua
        BT::Blackboard::Ptr blackboard;

        AIComponent() {
            blackboard = BT::Blackboard::create();
        }

        AIComponent(const std::string& path) : treePath(path) {
            blackboard = BT::Blackboard::create();
        }
    };
}
