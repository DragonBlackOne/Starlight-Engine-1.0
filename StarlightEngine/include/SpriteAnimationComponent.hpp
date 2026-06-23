#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace starlight {
    struct SpriteAnimationComponent {
        std::vector<uint32_t> frames;
        int currentFrame = 0;
        float fps = 12.0f;
        float timer = 0.0f;
        bool playing = true;
        bool looping = true;
    };
}
