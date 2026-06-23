#include "Shader.hpp"
#include "PathResolver.hpp"
#include "VFSSystem.hpp"
#include <glad/glad.h>
#include "Log.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace starlight {
    std::shared_ptr<Shader> Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
        auto loadSource = [](const std::string& path) -> std::string {
            auto bytes = VFSSystem::Get().ReadFile(path);
            if (bytes.empty()) {
                Log::Error("Failed to open shader file: " + path);
                return "";
            }
            return std::string(bytes.begin(), bytes.end());
        };

        std::string vCode = loadSource(vertexPath);
        std::string fCode = loadSource(fragmentPath);
        if (vCode.empty() || fCode.empty()) return nullptr;
        
        auto shader = std::make_shared<Shader>(vCode.c_str(), fCode.c_str());
        shader->m_vertexPath = vertexPath;
        shader->m_fragmentPath = fragmentPath;
        return shader;
    }
    Shader::Shader(const char* vertexSource, const char* fragmentSource) {
        uint32_t vertex, fragment;

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");

        // Shader Program
        m_id = glCreateProgram();
        glAttachShader(m_id, vertex);
        glAttachShader(m_id, fragment);
        glLinkProgram(m_id);
        CheckCompileErrors(m_id, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader::~Shader() {
        glDeleteProgram(m_id);
    }

    void Shader::Use() {
        glUseProgram(m_id);
    }

    void Shader::SetIntU(const char* name, int value) {
        glUniform1i(glGetUniformLocation(m_id, name), value);
    }

    void Shader::SetFloatU(const char* name, float value) {
        glUniform1f(glGetUniformLocation(m_id, name), value);
    }
    void Shader::SetVec2U(const char* name, const glm::vec2& value) {
        glUniform2fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec3U(const char* name, const glm::vec3& value) {
        glUniform3fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4U(const char* name, const glm::vec4& value) {
        glUniform4fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value));
    }

    void Shader::SetMat4U(const char* name, const glm::mat4& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetIntArray(const char* name, int* values, uint32_t count) {
        glUniform1iv(glGetUniformLocation(m_id, name), count, values);
    }

    void Shader::CheckCompileErrors(uint32_t shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Log::Error("Shader Compile Error (" + type + "): " + std::string(infoLog));
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Log::Error("Shader Link Error (" + type + "): " + std::string(infoLog));
            }
        }
    }

    bool Shader::CompileAndLink(const char* vSrc, const char* fSrc, uint32_t& outProgram, std::string& outError) {
        uint32_t vertex = 0, fragment = 0;
        int success = 0;
        char infoLog[1024];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vSrc, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
            outError = "VERTEX: " + std::string(infoLog);
            glDeleteShader(vertex);
            return false;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fSrc, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
            outError = "FRAGMENT: " + std::string(infoLog);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        outProgram = glCreateProgram();
        glAttachShader(outProgram, vertex);
        glAttachShader(outProgram, fragment);
        glLinkProgram(outProgram);
        glGetProgramiv(outProgram, GL_LINK_STATUS, &success);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        if (!success) {
            glGetProgramInfoLog(outProgram, 1024, NULL, infoLog);
            outError = "LINK: " + std::string(infoLog);
            glDeleteProgram(outProgram);
            outProgram = 0;
            return false;
        }

        return true;
    }

    bool Shader::Reload() {
        if (m_vertexPath.empty() || m_fragmentPath.empty()) {
            Log::Warn("Shader::Reload() called on shader without source paths (not created via LoadFromFile).");
            return false;
        }

        auto loadSource = [](const std::string& path) -> std::string {
            auto bytes = VFSSystem::Get().ReadFile(path);
            if (bytes.empty()) return "";
            return std::string(bytes.begin(), bytes.end());
        };

        std::string vCode = loadSource(m_vertexPath);
        std::string fCode = loadSource(m_fragmentPath);
        if (vCode.empty() || fCode.empty()) {
            Log::Error("Shader::Reload() failed to read source files: {} / {}", m_vertexPath, m_fragmentPath);
            return false;
        }

        uint32_t newProgram = 0;
        std::string error;
        if (!CompileAndLink(vCode.c_str(), fCode.c_str(), newProgram, error)) {
            Log::Error("Shader::Reload() compile/link failed (keeping old shader): {}", error);
            return false;
        }

        // Success: swap programs
        glDeleteProgram(m_id);
        m_id = newProgram;
        Log::Info("Shader hot-reloaded: {} + {}", m_vertexPath, m_fragmentPath);
        return true;
    }
}
