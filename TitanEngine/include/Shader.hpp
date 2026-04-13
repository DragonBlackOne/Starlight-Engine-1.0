#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace titan {
    class Shader {
    public:
        Shader(const char* vertexSource, const char* fragmentSource);
        ~Shader();

        void Use();
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

    private:
        uint32_t m_id;
        void CheckCompileErrors(uint32_t shader, std::string type);
    };
}
