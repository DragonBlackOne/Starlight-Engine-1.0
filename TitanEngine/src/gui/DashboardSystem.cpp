#include "DashboardSystem.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace titan {

    DashboardSystem::DashboardSystem() {}

    void DashboardSystem::Begin(int width, int height) {
        m_width = width;
        m_height = height;
        m_commands.clear();
    }

    bool DashboardSystem::Button(const std::string& label, float x, float y, float w, float h) {
        if (!m_enabled) return false;

        // Simple mouse check
        int mx, my;
        uint32_t mouseState = SDL_GetMouseState(&mx, &my);
        bool hover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
        bool click = hover && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

        glm::vec4 color = glm::vec4(0.2f, 0.2f, 0.25f, 0.9f);
        if (hover) color = glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);
        if (click) color = glm::vec4(0.1f, 0.6f, 0.9f, 1.0f);

        m_commands.push_back({ UICommand::BUTTON, x, y, w, h, color, label });
        return click;
    }

    void DashboardSystem::Label(const std::string& text, float x, float y, const glm::vec4& color) {
        if (!m_enabled) return;
        m_commands.push_back({ UICommand::LABEL, x, y, 0, 0, color, text });
    }

    void DashboardSystem::Panel(float x, float y, float w, float h, const glm::vec4& color) {
        if (!m_enabled) return;
        m_commands.push_back({ UICommand::PANEL, x, y, w, h, color, "" });
    }

    void DashboardSystem::End(Renderer& renderer) {
        if (!m_enabled || m_commands.empty()) return;

        glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f, -1.0f, 1.0f);
        auto shader = renderer.GetBasicShader();
        shader->Use();
        shader->SetMat4("view", glm::mat4(1.0f));
        shader->SetMat4("projection", projection);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& cmd : m_commands) {
            if (cmd.type == UICommand::PANEL || cmd.type == UICommand::BUTTON) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cmd.x + cmd.w/2, cmd.y + cmd.h/2, 0.0f));
                model = glm::scale(model, glm::vec3(cmd.w, cmd.h, 1.0f));
                
                shader->SetMat4("model", model);
                shader->SetVec3("color", glm::vec3(cmd.color));
                // We assume renderer has a shared cube or quad for this.
                // Using Cube as a fallback if quad is not exposed or just draw quad.
                renderer.GetCubeMesh()->Draw(); 
            }
            // Note: Text rendering would go here, requiring a font system.
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

}
