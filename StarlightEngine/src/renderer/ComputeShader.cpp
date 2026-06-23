#include "ComputeShader.hpp"
#include "PathResolver.hpp"
#include "VFSSystem.hpp"
#include "Log.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace starlight {

    ComputeShader::ComputeShader(const std::string& computePath) {
        std::string computeCode;
        try {
            auto bytes = VFSSystem::Get().ReadFile(computePath);
            if (bytes.empty()) {
                Log::Error("ComputeShader: Failed to open file: " + computePath);
                return;
            }
            computeCode = std::string(bytes.begin(), bytes.end());
            
            // Strip UTF-8 BOM if present
            if (computeCode.size() >= 3 && 
                (unsigned char)computeCode[0] == 0xEF && 
                (unsigned char)computeCode[1] == 0xBB && 
                (unsigned char)computeCode[2] == 0xBF) {
                computeCode = computeCode.substr(3);
            }
        } catch (std::exception& e) {
            Log::Error("ComputeShader: Exception reading file " + computePath + ": " + e.what());
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
        if (m_programID) glDeleteProgram(m_programID);
    }

    void ComputeShader::Use() {
        if (m_programID) glUseProgram(m_programID);
    }

    void ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z) {
        if (m_programID) glDispatchCompute(x, y, z);
    }

    void ComputeShader::Wait() {
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    void ComputeShader::SetIntU(const std::string& name, int value) {
        if (!m_programID) return;
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
    }

    void ComputeShader::SetFloatU(const std::string& name, float value) {
        if (!m_programID) return;
        glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
    }

    void ComputeShader::SetVec3U(const std::string& name, const glm::vec3& value) {
        if (!m_programID) return;
        glUniform3f(glGetUniformLocation(m_programID, name.c_str()), value.x, value.y, value.z);
    }

    void ComputeShader::CheckCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Log::Error("ComputeShader: Compilation Error (" + type + "):\n" + std::string(infoLog));
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Log::Error("ComputeShader: Linking Error (" + type + "):\n" + std::string(infoLog));
            }
        }
    }

}
