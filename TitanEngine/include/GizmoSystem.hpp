#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.hpp"

namespace titan {

    class GizmoSystem {
    public:
        GizmoSystem();
        ~GizmoSystem();

        void Initialize();
        void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection);
        void DrawCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection);

    private:
        uint32_t m_vao = 0, m_vbo = 0;
        std::shared_ptr<Shader> m_gizmoShader;
    };

}
