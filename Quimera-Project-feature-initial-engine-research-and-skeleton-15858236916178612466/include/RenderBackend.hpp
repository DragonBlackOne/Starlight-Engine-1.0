#pragma once
#include <vector>
#include <functional>
#include <memory>
#include "MeshComponent.hpp"
#include "MaterialComponent.hpp"
#include <glm/glm.hpp>

namespace Vamos {
    struct RenderCommand {
        Mesh* mesh;
        glm::mat4 model;
        MaterialComponent* material = nullptr;
        std::vector<glm::mat4> skinningMatrices;
        bool hasAnimation = false;
    };

    class RenderView {
    public:
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        std::vector<RenderCommand> commands;

        void AddCommand(const RenderCommand& cmd) {
            commands.push_back(cmd);
        }

        void Clear() {
            commands.clear();
        }
    };

    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        virtual void Initialize() = 0;
        virtual void Render(const RenderView& view) = 0;
        virtual void Shutdown() = 0;
    };
}
