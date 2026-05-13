#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace starlight {

    class Shader {
    public:
        Shader(const char* vertexSource, const char* fragmentSource);
        ~Shader();

        static std::shared_ptr<Shader> LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

        void Use();
        void SetIntU(const char* name, int value);
        void SetFloatU(const char* name, float value);
        void SetVec2U(const char* name, const glm::vec2& value);
        void SetVec3U(const char* name, const glm::vec3& value);
        void SetVec4U(const char* name, const glm::vec4& value);
        void SetMat4U(const char* name, const glm::mat4& value);
        void SetIntArray(const char* name, int* values, uint32_t count);

        // std::string overloads
        void SetIntU(const std::string& name, int value) { SetIntU(name.c_str(), value); }
        void SetFloatU(const std::string& name, float value) { SetFloatU(name.c_str(), value); }
        void SetVec2U(const std::string& name, const glm::vec2& value) { SetVec2U(name.c_str(), value); }
        void SetVec3U(const std::string& name, const glm::vec3& value) { SetVec3U(name.c_str(), value); }
        void SetVec4U(const std::string& name, const glm::vec4& value) { SetVec4U(name.c_str(), value); }
        void SetMat4U(const std::string& name, const glm::mat4& value) { SetMat4U(name.c_str(), value); }

    private:
        uint32_t m_id;
        void CheckCompileErrors(uint32_t shader, std::string type);
    };

}
