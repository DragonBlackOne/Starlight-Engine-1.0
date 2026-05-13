// Este projeto ÃƒÂ© feito por IA e sÃƒÂ³ o prompt ÃƒÂ© feito por um humano.
#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "EngineSystem.hpp"
#include "PAL.hpp"

namespace starlight {
    struct InputAction {
        std::string name;
        std::vector<pal::KeyCode> keys;
        std::vector<pal::MouseButton> mouseButtons;
    };

    class InputSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnUpdate(float dt) override;
        void OnShutdown() override {}
        const char* GetName() const override { return "InputSystem"; }

        // Semantic Action Checks
        bool IsActionPressed(const std::string& name) const;
        bool IsActionJustPressed(const std::string& name) const;
        
        static bool IsKeyPressed(pal::KeyCode key);
        static bool IsKeyJustPressed(pal::KeyCode key);

        void BindAction(const std::string& name, pal::KeyCode key);
        void BindMouseButton(const std::string& name, pal::MouseButton button);

        glm::vec2 GetMousePosition() const { return m_mousePos; }

    private:
        std::unordered_map<std::string, InputAction> m_actions;
        static uint8_t s_currKeys[512]; // Use fixed size for PAL mapping
        static uint8_t s_prevKeys[512];
        static uint32_t s_currMouse;
        static uint32_t s_prevMouse;
        glm::vec2 m_mousePos;
    };
}
