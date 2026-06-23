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
        void OnShutdown() override;
        const char* GetName() const override { return "InputSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        static pal::KeyCode KeyCodeFromString(const std::string& name);
        static std::string StringFromKeyCode(pal::KeyCode key);

        // Semantic Action Checks
        bool IsActionPressed(const std::string& name) const;
        bool IsActionJustPressed(const std::string& name) const;
        
        static bool IsKeyPressed(pal::KeyCode key);
        static bool IsKeyJustPressed(pal::KeyCode key);

        void BindAction(const std::string& name, pal::KeyCode key);
        void BindMouseButton(const std::string& name, pal::MouseButton button);
        void SaveBindings(class ConfigSystem& config) const;

        glm::vec2 GetMousePosition() const { return m_mousePos; }
        
        // Gamepad API (Input 2.0)
        float GetAxis(const std::string& axisName) const;
        bool IsGamepadButtonPressed(const std::string& buttonName) const;
        void Vibrate(float leftMotor, float rightMotor, uint32_t durationMS);

    private:
        std::unordered_map<std::string, InputAction> m_actions;
        static uint8_t s_currKeys[512]; // Use fixed size for PAL mapping
        static uint8_t s_prevKeys[512];
        static uint32_t s_currMouse;
        static uint32_t s_prevMouse;
        glm::vec2 m_mousePos;

        void* m_gamepad = nullptr; // SDL_GameController*
        float m_axes[6] = {0};
        uint8_t m_buttons[15] = {0};
    };
}
