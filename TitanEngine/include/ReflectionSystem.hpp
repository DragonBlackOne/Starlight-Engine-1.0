// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>
#include <glm/glm.hpp>

namespace titan {

    enum class PropertyType {
        Float,
        Int,
        Vec3,
        String,
        Bool
    };

    struct PropertyMetadata {
        std::string name;
        PropertyType type;
        size_t offset;
    };

    struct ComponentMetadata {
        std::string name;
        std::vector<PropertyMetadata> properties;
    };

    class ReflectionSystem {
    public:
        static ReflectionSystem& Get();

        template<typename T>
        void RegisterComponent(const std::string& name, const std::vector<PropertyMetadata>& props) {
            m_components[name] = { name, props };
        }

        const std::map<std::string, ComponentMetadata>& GetComponents() const { return m_components; }

    private:
        ReflectionSystem() = default;
        std::map<std::string, ComponentMetadata> m_components;
    };

}
