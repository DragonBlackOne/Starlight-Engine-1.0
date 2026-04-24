// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace titan {

    class ComputeShader {
    public:
        ComputeShader(const std::string& computePath);
        ~ComputeShader();

        void Use();
        void Dispatch(unsigned int x, unsigned int y, unsigned int z);
        void Wait();

        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec3(const std::string& name, const glm::vec3& value);

    private:
        unsigned int m_programID;
        void CheckCompileErrors(unsigned int shader, std::string type);
    };

}
