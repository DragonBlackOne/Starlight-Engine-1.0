#include "RmlRenderInterfaceGL3.hpp"
#include <glad/glad.h>
#include <RmlUi/Core.h>
#include "Log.hpp"
#include "Engine.hpp"

namespace starlight {

    struct RmlCompiledGeometry {
        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint IBO = 0;
        int numIndices = 0;
        Rml::TextureHandle texture = 0;
    };

    RmlRenderInterfaceGL3::RmlRenderInterfaceGL3() {
        SetupShader();
    }

    RmlRenderInterfaceGL3::~RmlRenderInterfaceGL3() {
        if (m_shader != 0) {
            glDeleteProgram(m_shader);
        }
    }

    void RmlRenderInterfaceGL3::SetupShader() {
        const char* vertexShaderSrc = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec4 aColor;
            layout (location = 2) in vec2 aTexCoord;

            out vec4 FragColor;
            out vec2 TexCoord;

            uniform vec2 uTranslation;
            uniform mat4 uProjection;

            void main() {
                FragColor = aColor;
                TexCoord = aTexCoord;
                gl_Position = uProjection * vec4(aPos + uTranslation, 0.0, 1.0);
            }
        )";

        const char* fragmentShaderSrc = R"(
            #version 330 core
            in vec4 FragColor;
            in vec2 TexCoord;
            out vec4 outColor;

            uniform sampler2D uTexture;
            uniform int uUseTexture;

            void main() {
                if (uUseTexture != 0) {
                    outColor = texture(uTexture, TexCoord) * FragColor;
                } else {
                    outColor = FragColor;
                }
            }
        )";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragmentShaderSrc, nullptr);
        glCompileShader(fs);

        m_shader = glCreateProgram();
        glAttachShader(m_shader, vs);
        glAttachShader(m_shader, fs);
        glLinkProgram(m_shader);

        glDeleteShader(vs);
        glDeleteShader(fs);

        m_shaderTranslationLocation = glGetUniformLocation(m_shader, "uTranslation");
        m_shaderUseTextureLocation = glGetUniformLocation(m_shader, "uUseTexture");
    }

    void RmlRenderInterfaceGL3::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) {
        // Backup GL state
        GLint last_program = 0; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_vao = 0; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vao);
        GLint last_vbo = 0; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_vbo);
        GLint last_ibo = 0; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_ibo);
        GLint last_texture = 0; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLboolean last_blend = glIsEnabled(GL_BLEND);
        GLboolean last_cull = glIsEnabled(GL_CULL_FACE);
        GLboolean last_depth = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_scissor = glIsEnabled(GL_SCISSOR_TEST);
        GLint last_blend_src_alpha = 0; glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
        GLint last_blend_dst_alpha = 0; glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
        GLint last_blend_src_rgb = 0; glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
        GLint last_blend_dst_rgb = 0; glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
        GLint last_scissor_box[4] = { 0 }; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

        // Renderização dinâmica rápida (sem compilar)
        GLuint vao = 0, vbo = 0, ibo = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Rml::Vertex), vertices, GL_STREAM_DRAW);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(int), indices, GL_STREAM_DRAW);

        glEnableVertexAttribArray(0); // pos
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const void*)offsetof(Rml::Vertex, position));

        glEnableVertexAttribArray(1); // color
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rml::Vertex), (const void*)offsetof(Rml::Vertex, colour));

        glEnableVertexAttribArray(2); // texCoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const void*)offsetof(Rml::Vertex, tex_coord));

        // Get viewport size for projection matrix
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        m_viewportWidth = viewport[2];
        m_viewportHeight = viewport[3];

        glUseProgram(m_shader);
        glm::mat4 projection = glm::ortho(0.0f, (float)m_viewportWidth, (float)m_viewportHeight, 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjection"), 1, GL_FALSE, &projection[0][0]);
        glUniform2f(m_shaderTranslationLocation, translation.x, translation.y);

        if (texture != 0) {
            glUniform1i(m_shaderUseTextureLocation, 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
        } else {
            glUniform1i(m_shaderUseTextureLocation, 0);
        }

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (m_scissorEnabled) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(m_scissorX, m_viewportHeight - (m_scissorY + m_scissorHeight), m_scissorWidth, m_scissorHeight);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);

        // Restore state
        glUseProgram(last_program);
        glBindVertexArray(last_vao);
        glBindBuffer(GL_ARRAY_BUFFER, last_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_ibo);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        
        if (last_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        
        if (last_cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_depth) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        
        if (last_scissor) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }

    Rml::CompiledGeometryHandle RmlRenderInterfaceGL3::CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) {
        RmlCompiledGeometry* geom = new RmlCompiledGeometry();
        geom->numIndices = num_indices;
        geom->texture = texture;

        glGenVertexArrays(1, &geom->VAO);
        glBindVertexArray(geom->VAO);

        glGenBuffers(1, &geom->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, geom->VBO);
        glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Rml::Vertex), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &geom->IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(int), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // pos
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const void*)offsetof(Rml::Vertex, position));

        glEnableVertexAttribArray(1); // color
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rml::Vertex), (const void*)offsetof(Rml::Vertex, colour));

        glEnableVertexAttribArray(2); // texCoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const void*)offsetof(Rml::Vertex, tex_coord));

        glBindVertexArray(0);
        return (Rml::CompiledGeometryHandle)geom;
    }

    void RmlRenderInterfaceGL3::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) {
        RmlCompiledGeometry* geom = (RmlCompiledGeometry*)geometry;
        if (!geom) return;

        // Backup GL state
        GLint last_program = 0; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_vao = 0; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vao);
        GLint last_vbo = 0; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_vbo);
        GLint last_ibo = 0; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_ibo);
        GLint last_texture = 0; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLboolean last_blend = glIsEnabled(GL_BLEND);
        GLboolean last_cull = glIsEnabled(GL_CULL_FACE);
        GLboolean last_depth = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_scissor = glIsEnabled(GL_SCISSOR_TEST);
        GLint last_blend_src_alpha = 0; glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
        GLint last_blend_dst_alpha = 0; glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
        GLint last_blend_src_rgb = 0; glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
        GLint last_blend_dst_rgb = 0; glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
        GLint last_scissor_box[4] = { 0 }; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        m_viewportWidth = viewport[2];
        m_viewportHeight = viewport[3];

        glUseProgram(m_shader);
        glm::mat4 projection = glm::ortho(0.0f, (float)m_viewportWidth, (float)m_viewportHeight, 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjection"), 1, GL_FALSE, &projection[0][0]);
        glUniform2f(m_shaderTranslationLocation, translation.x, translation.y);

        if (geom->texture != 0) {
            glUniform1i(m_shaderUseTextureLocation, 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, (GLuint)geom->texture);
        } else {
            glUniform1i(m_shaderUseTextureLocation, 0);
        }

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (m_scissorEnabled) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(m_scissorX, m_viewportHeight - (m_scissorY + m_scissorHeight), m_scissorWidth, m_scissorHeight);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        glBindVertexArray(geom->VAO);
        glDrawElements(GL_TRIANGLES, geom->numIndices, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // Restore state
        glUseProgram(last_program);
        glBindVertexArray(last_vao);
        glBindBuffer(GL_ARRAY_BUFFER, last_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_ibo);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        
        if (last_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        
        if (last_cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_depth) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        
        if (last_scissor) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void RmlRenderInterfaceGL3::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) {
        RmlCompiledGeometry* geom = (RmlCompiledGeometry*)geometry;
        if (geom) {
            glDeleteBuffers(1, &geom->VBO);
            glDeleteBuffers(1, &geom->IBO);
            glDeleteVertexArrays(1, &geom->VAO);
            delete geom;
        }
    }

    bool RmlRenderInterfaceGL3::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {
        GLuint texID = 0;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        texture_handle = (Rml::TextureHandle)texID;
        return true;
    }

    void RmlRenderInterfaceGL3::ReleaseTexture(Rml::TextureHandle texture_handle) {
        if (texture_handle != 0) {
            GLuint texID = (GLuint)texture_handle;
            glDeleteTextures(1, &texID);
        }
    }

    void RmlRenderInterfaceGL3::EnableScissorRegion(bool enable) {
        m_scissorEnabled = enable;
    }

    void RmlRenderInterfaceGL3::SetScissorRegion(int x, int y, int width, int height) {
        m_scissorX = x;
        m_scissorY = y;
        m_scissorWidth = width;
        m_scissorHeight = height;
    }

}
