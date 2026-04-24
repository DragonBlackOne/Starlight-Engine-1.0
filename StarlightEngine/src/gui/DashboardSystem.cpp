// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "DashboardSystem.hpp"
#include "Renderer.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace starlight {

    DashboardSystem::DashboardSystem() {}

    DashboardSystem::~DashboardSystem() {
        if (m_fontTexture) glDeleteTextures(1, &m_fontTexture);
        if (m_fontVao) glDeleteVertexArrays(1, &m_fontVao);
        if (m_fontVbo) glDeleteBuffers(1, &m_fontVbo);
    }

    void DashboardSystem::Initialize() {
        // Load font
        FILE* file;
        fopen_s(&file, "C:/Windows/Fonts/arial.ttf", "rb");
        if (!file) {
            Log::Error("DashboardSystem: Failed to open C:/Windows/Fonts/arial.ttf");
            return;
        }

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        unsigned char* ttf_buffer = new unsigned char[size];
        fread(ttf_buffer, 1, size, file);
        fclose(file);

        unsigned char* temp_bitmap = new unsigned char[512 * 512];
        stbtt_BakeFontBitmap(ttf_buffer, 0, 24.0f, temp_bitmap, 512, 512, 32, 96, m_cdata);

        delete[] ttf_buffer;

        glGenTextures(1, &m_fontTexture);
        glBindTexture(GL_TEXTURE_2D, m_fontTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        delete[] temp_bitmap;

        glGenVertexArrays(1, &m_fontVao);
        glGenBuffers(1, &m_fontVbo);
        glBindVertexArray(m_fontVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    void DashboardSystem::Begin(int width, int height) {
        m_width = width;
        m_height = height;
        m_commands.clear();
    }

    bool DashboardSystem::Button(const std::string& label, float x, float y, float w, float h) {
        if (!m_enabled) return false;

        int mx, my;
        uint32_t mouseState = SDL_GetMouseState(&mx, &my);
        bool hover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
        bool click = hover && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

        glm::vec4 color = glm::vec4(0.2f, 0.2f, 0.25f, 0.9f);
        if (hover) color = glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);
        if (click) color = glm::vec4(0.1f, 0.6f, 0.9f, 1.0f);

        m_commands.push_back({ UICommand::BUTTON, x, y, w, h, color, label });
        // Add label explicitly inside button
        Label(label, x + 10, y + h/2 + 5, glm::vec4(1,1,1,1));
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
        auto shader = renderer.GetUIShader();
        if(!shader) return;

        shader->Use();
        shader->SetMat4("view", glm::mat4(1.0f));
        shader->SetMat4("projection", projection);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& cmd : m_commands) {
            if (cmd.type == UICommand::PANEL || cmd.type == UICommand::BUTTON) {
                shader->SetInt("uUseTexture", 0);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cmd.x + cmd.w/2, cmd.y + cmd.h/2, 0.0f));
                model = glm::scale(model, glm::vec3(cmd.w, cmd.h, 1.0f));
                
                shader->SetMat4("model", model);
                shader->SetVec4("uColor", cmd.color);
                renderer.GetCubeMesh()->Draw(); 
            }
            else if (cmd.type == UICommand::LABEL) {
                if (!m_fontTexture) continue;
                shader->SetInt("uUseTexture", 1);
                shader->SetVec4("uColor", cmd.color);
                shader->SetMat4("model", glm::mat4(1.0f));
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_fontTexture);
                shader->SetInt("uTexture", 0);

                glBindVertexArray(m_fontVao);
                glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);

                float cx = cmd.x;
                float cy = cmd.y;
                for (size_t i = 0; i < cmd.text.length(); i++) {
                    if (cmd.text[i] >= 32 && cmd.text[i] < 128) {
                        stbtt_aligned_quad q;
                        stbtt_GetBakedQuad(m_cdata, 512, 512, cmd.text[i] - 32, &cx, &cy, &q, 1);
                        float vertices[6][5] = {
                            { q.x0, q.y1, 0.0f, q.s0, q.t1 },
                            { q.x1, q.y0, 0.0f, q.s1, q.t0 },
                            { q.x0, q.y0, 0.0f, q.s0, q.t0 },

                            { q.x0, q.y1, 0.0f, q.s0, q.t1 },
                            { q.x1, q.y1, 0.0f, q.s1, q.t1 },
                            { q.x1, q.y0, 0.0f, q.s1, q.t0 }
                        };
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                    }
                }
                glBindVertexArray(0);
            }
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

}
