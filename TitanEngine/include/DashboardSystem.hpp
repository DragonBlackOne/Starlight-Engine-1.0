#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "stb_truetype.h"

namespace titan {

    class Renderer;

    class DashboardSystem {
    public:
        DashboardSystem();
        ~DashboardSystem();
        
        void Initialize();
        void Begin(int width, int height);
        bool Button(const std::string& label, float x, float y, float w, float h);
        void Label(const std::string& text, float x, float y, const glm::vec4& color = glm::vec4(1,1,1,1));
        void Panel(float x, float y, float w, float h, const glm::vec4& color);
        void End(Renderer& renderer);

        bool IsEnabled() const { return m_enabled; }
        void Toggle() { m_enabled = !m_enabled; }

    private:
        bool m_enabled = true;
        int m_width, m_height;
        
        struct UICommand {
            enum Type { PANEL, LABEL, BUTTON };
            Type type;
            float x, y, w, h;
            glm::vec4 color;
            std::string text;
        };
        std::vector<UICommand> m_commands;

        uint32_t m_fontTexture = 0;
        stbtt_bakedchar m_cdata[96];
        uint32_t m_fontVao = 0, m_fontVbo = 0;
    };

}
