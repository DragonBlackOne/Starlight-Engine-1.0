// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "GizmoSystem.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace starlight {

    GizmoSystem::GizmoSystem() {}

    GizmoSystem::~GizmoSystem() {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
    }

    void GizmoSystem::Initialize() {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        m_gizmoShader = std::make_shared<Shader>(
            "#version 450 core\n"
            "layout(location = 0) in vec3 aPos;\n"
            "uniform mat4 mvp;\n"
            "void main() { gl_Position = mvp * vec4(aPos, 1.0); }",
            "#version 450 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 gizmoColor;\n"
            "void main() { FragColor = vec4(gizmoColor, 1.0); }"
        );
    }

    void GizmoSystem::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection) {
        float verts[] = { start.x, start.y, start.z, end.x, end.y, end.z };
        glm::mat4 mvp = projection * view;

        m_gizmoShader->Use();
        m_gizmoShader->SetMat4("mvp", mvp);
        m_gizmoShader->SetVec3("gizmoColor", color);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glDrawArrays(GL_LINES, 0, 2);
    }

    void GizmoSystem::DrawCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection) {
        glm::vec3 x1 = center - size * 0.5f;
        glm::vec3 x2 = center + size * 0.5f;

        glm::vec3 v[8] = {
            {x1.x, x1.y, x1.z}, {x2.x, x1.y, x1.z}, {x2.x, x2.y, x1.z}, {x1.x, x2.y, x1.z},
            {x1.x, x1.y, x2.z}, {x2.x, x1.y, x2.z}, {x2.x, x2.y, x2.z}, {x1.x, x2.y, x2.z}
        };

        // Draw cube edges using DrawLine
        for (int i = 0; i < 4; i++) {
            DrawLine(v[i], v[(i + 1) % 4], color, view, projection);       // Bottom
            DrawLine(v[i + 4], v[((i + 1) % 4) + 4], color, view, projection); // Top
            DrawLine(v[i], v[i + 4], color, view, projection);             // Verticals
        }
    }

}
