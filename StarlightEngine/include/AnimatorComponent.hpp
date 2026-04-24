// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace starlight {

    struct AnimatorComponent {
        std::vector<glm::mat4> jointMatrices;
        bool active = false;
        
        static constexpr int MAX_JOINTS = 64;

        AnimatorComponent() {
            jointMatrices.resize(MAX_JOINTS, glm::mat4(1.0f));
        }
    };

}
