// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.hpp"

namespace starlight {

    struct LODLevel {
        std::shared_ptr<Mesh> mesh;
        float distance; // Transition threshold
    };

    struct LODComponent {
        std::vector<LODLevel> levels;
        int currentLevel = 0;

        void AddLevel(std::shared_ptr<Mesh> mesh, float distance) {
            levels.push_back({mesh, distance});
        }
    };

}
