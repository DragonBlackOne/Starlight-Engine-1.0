#pragma once
#include <string>
#include <memory>
#include "Renderer.hpp"

namespace titan {
    class AssetLoader {
    public:
        static std::shared_ptr<Mesh> LoadOBJ(const std::string& path);
        static uint32_t LoadTexture(const std::string& path);
        static uint32_t CreateCheckerboardTexture(int width, int height, int cellSize);
    };
}
