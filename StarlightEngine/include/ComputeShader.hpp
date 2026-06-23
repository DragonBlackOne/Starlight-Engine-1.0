#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace starlight {

    class ComputeShader {
    public:
        ComputeShader(const std::string& computePath);
        ~ComputeShader();

        void Use();
        void Dispatch(unsigned int x, unsigned int y, unsigned int z);
        void Wait();

        void SetIntU(const std::string& name, int value);
        void SetFloatU(const std::string& name, float value);
        void SetVec3U(const std::string& name, const glm::vec3& value);

    private:
        unsigned int m_programID = 0;
        void CheckCompileErrors(unsigned int shader, std::string type);
    };

}
