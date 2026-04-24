// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vamos {
    class Shader {
    public:
        unsigned int ID;

        Shader(const char* vertexPath, const char* fragmentPath) {
            std::string vertexCode;
            std::string fragmentCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;

            // Tentativa de caminhos relativos ao executável (comumente assets/...)
            // ou ao root se rodando do root.
            const char* paths[] = { vertexPath, fragmentPath };
            std::string* codes[] = { &vertexCode, &fragmentCode };

            for(int i=0; i<2; ++i) {
                std::ifstream file;
                file.open(paths[i]);
                if(!file.is_open()) {
                    // Tentar prefixo ../ se estiver dentro de build/
                    std::string altPath = std::string("../") + paths[i];
                    file.open(altPath);
                    if(!file.is_open()) {
                        std::cerr << "ERRO::SHADER::ARQUIVO_NAO_ENCONTRADO: " << paths[i] << std::endl;
                        continue;
                    }
                }
                std::stringstream stream;
                stream << file.rdbuf();
                file.close();
                *codes[i] = stream.str();
            }

            if(vertexCode.empty() || fragmentCode.empty()) {
                 std::cerr << "ERRO::SHADER::CODIGO_VAZIO" << std::endl;
                 return;
            }

            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

            unsigned int vertex, fragment;
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            CheckCompileErrors(vertex, "VERTEX");

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            CheckCompileErrors(fragment, "FRAGMENT");

            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            glLinkProgram(ID);
            CheckCompileErrors(ID, "PROGRAM");

            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }

        void Use() { glUseProgram(ID); }

        void SetMat4(const std::string& name, const glm::mat4& mat) const {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
        }

        void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& mats) const {
            if (mats.empty()) return;
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), (GLsizei)mats.size(), GL_FALSE, glm::value_ptr(mats[0]));
        }

        void SetVec4(const std::string& name, const glm::vec4& vec) const {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
        }

        void SetVec3(const std::string& name, const glm::vec3& vec) const {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
        }

        void SetBool(const std::string& name, bool value) const {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }

        void SetInt(const std::string& name, int value) const {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }

        void SetFloat(const std::string& name, float value) const {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }

    private:
        void CheckCompileErrors(unsigned int shader, std::string type) {
            int success;
            char infoLog[1024];
            if (type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cerr << "ERRO::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            } else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cerr << "ERRO::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
        }
    };
}
