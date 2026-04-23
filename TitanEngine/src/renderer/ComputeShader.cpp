#include "ComputeShader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace titan {

    ComputeShader::ComputeShader(const std::string& computePath) {
        std::string computeCode;
        std::ifstream cShaderFile;
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;
            cShaderStream << cShaderFile.rdbuf();
            cShaderFile.close();
            computeCode = cShaderStream.str();
        } catch (std::exception& e) {
            (void)e;
            std::cerr << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }
        const char* cShaderCode = computeCode.c_str();

        unsigned int compute;
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        CheckCompileErrors(compute, "COMPUTE");

        m_programID = glCreateProgram();
        glAttachShader(m_programID, compute);
        glLinkProgram(m_programID);
        CheckCompileErrors(m_programID, "PROGRAM");

        glDeleteShader(compute);
    }

    ComputeShader::~ComputeShader() {
        glDeleteProgram(m_programID);
    }

    void ComputeShader::Use() {
        glUseProgram(m_programID);
    }

    void ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z) {
        glDispatchCompute(x, y, z);
    }

    void ComputeShader::Wait() {
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    void ComputeShader::SetInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
    }

    void ComputeShader::SetFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
    }

    void ComputeShader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3f(glGetUniformLocation(m_programID, name.c_str()), value.x, value.y, value.z);
    }

    void ComputeShader::CheckCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }

}
