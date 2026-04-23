#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace titan {
    class Shader {
    public:
        Shader(const char* vertexSource, const char* fragmentSource);
        ~Shader();

        static std::shared_ptr<Shader> LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

        void Use();
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

    private:
        uint32_t m_id;
        void CheckCompileErrors(uint32_t shader, std::string type);
    };
}
