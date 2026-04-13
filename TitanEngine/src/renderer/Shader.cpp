#include "Shader.hpp"
#include "Log.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace titan {
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

    void Shader::SetInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::SetFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
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
}
