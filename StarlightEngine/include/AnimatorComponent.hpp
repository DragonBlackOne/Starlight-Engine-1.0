// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
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
