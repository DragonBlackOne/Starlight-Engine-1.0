// Este projeto Ã© feito por IA e sÃ³ o prompt Ã© feito por um humano.
#include "InputSystem.hpp"
#include <cstring>
#include <SDL2/SDL.h>
#include <algorithm>

namespace starlight {

    static SDL_Scancode ToSDLScancode(pal::KeyCode key) {
        return static_cast<SDL_Scancode>(key);
    }

    uint8_t InputSystem::s_currKeys[512] = {0};
    uint8_t InputSystem::s_prevKeys[512] = {0};
    uint32_t InputSystem::s_currMouse = 0;
    uint32_t InputSystem::s_prevMouse = 0;

    bool InputSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        std::memset(s_currKeys, 0, 512);
        std::memset(s_prevKeys, 0, 512);

        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == 0) {
            for (int i = 0; i < SDL_NumJoysticks(); ++i) {
                if (SDL_IsGameController(i)) {
                    m_gamepad = SDL_GameControllerOpen(i);
                    if (m_gamepad) break;
                }
            }
        }

        // Professional Default Bindings (Using PAL KeyCodes)
        BindAction("Jump", pal::KeyCode::Space);
        BindAction("MoveForward", pal::KeyCode::W);
        BindAction("MoveBackward", pal::KeyCode::S);
        BindAction("MoveLeft", pal::KeyCode::A);
        BindAction("MoveRight", pal::KeyCode::D);
        BindAction("EditorToggle", pal::KeyCode::F2);
        
        // Universal SBA Bindings
        BindAction("W", pal::KeyCode::W); BindAction("A", pal::KeyCode::A);
        BindAction("S", pal::KeyCode::S); BindAction("D", pal::KeyCode::D);
        BindAction("R", pal::KeyCode::R); BindAction("E", pal::KeyCode::E);
        BindAction("F", pal::KeyCode::F); BindAction("Q", pal::KeyCode::Q);
        BindAction("C", pal::KeyCode::C); BindAction("Shift", pal::KeyCode::LShift);
        BindAction("Space", pal::KeyCode::Space);
        BindAction("Escape", pal::KeyCode::Escape);
        
        // Arrow Keys
        BindAction("Up", pal::KeyCode::Up);
        BindAction("Down", pal::KeyCode::Down);
        BindAction("Left", pal::KeyCode::Left);
        BindAction("Right", pal::KeyCode::Right);
        
        // Numeric Keys for Menus
        BindAction("1", pal::KeyCode::Num1); BindAction("2", pal::KeyCode::Num2);
        BindAction("3", pal::KeyCode::Num3); BindAction("4", pal::KeyCode::Num4);
        BindAction("5", pal::KeyCode::Num5); BindAction("6", pal::KeyCode::Num6);
        BindAction("7", pal::KeyCode::Num7); BindAction("8", pal::KeyCode::Num8);
        BindAction("9", pal::KeyCode::Num9); BindAction("0", pal::KeyCode::Num0);
        BindAction("Console", pal::KeyCode::F1);
        
        BindMouseButton("MouseLeft", pal::MouseButton::Left);

        return true;
    }

    void InputSystem::OnUpdate(float dt) {
        (void)dt;
        std::memcpy(s_prevKeys, s_currKeys, 512);
        const uint8_t* state = SDL_GetKeyboardState(NULL);
        std::memcpy(s_currKeys, state, std::min((int)SDL_NUM_SCANCODES, 512));

        s_prevMouse = s_currMouse;
        int mx, my;
        s_currMouse = SDL_GetMouseState(&mx, &my);
        m_mousePos = glm::vec2(mx, my);

        // Update Gamepad
        if (m_gamepad) {
            auto controller = (SDL_GameController*)m_gamepad;
            for (int i = 0; i < 6; i++) {
                int16_t val = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)i);
                m_axes[i] = (float)val / 32767.0f;
                // Deadzone
                if (std::abs(m_axes[i]) < 0.15f) m_axes[i] = 0.0f;
            }
            for (int i = 0; i < 15; i++) {
                m_buttons[i] = SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)i);
            }
        }
    }

    bool InputSystem::IsActionPressed(const std::string& name) const {
        auto it = m_actions.find(name);
        if (it != m_actions.end()) {
            for (auto key : it->second.keys) {
                if (IsKeyPressed(key)) return true;
            }
            for (auto button : it->second.mouseButtons) {
                if (s_currMouse & SDL_BUTTON(static_cast<uint8_t>(button))) return true;
            }
        }
        return false;
    }

    bool InputSystem::IsActionJustPressed(const std::string& name) const {
        auto it = m_actions.find(name);
        if (it != m_actions.end()) {
            for (auto key : it->second.keys) {
                if (IsKeyJustPressed(key)) return true;
            }
            for (auto button : it->second.mouseButtons) {
                uint32_t mask = SDL_BUTTON(static_cast<uint8_t>(button));
                if ((s_currMouse & mask) && !(s_prevMouse & mask)) return true;
            }
        }
        return false;
    }

    bool InputSystem::IsKeyPressed(pal::KeyCode key) {
        return s_currKeys[ToSDLScancode(key)] != 0;
    }

    bool InputSystem::IsKeyJustPressed(pal::KeyCode key) {
        return (s_currKeys[ToSDLScancode(key)] != 0) && (s_prevKeys[ToSDLScancode(key)] == 0);
    }

    void InputSystem::BindAction(const std::string& name, pal::KeyCode key) {
        m_actions[name].name = name;
        m_actions[name].keys.push_back(key);
    }

    void InputSystem::BindMouseButton(const std::string& name, pal::MouseButton button) {
        m_actions[name].name = name;
        m_actions[name].mouseButtons.push_back(button);
    }

    float InputSystem::GetAxis(const std::string& axisName) const {
        if (axisName == "LeftX") return m_axes[SDL_CONTROLLER_AXIS_LEFTX];
        if (axisName == "LeftY") return m_axes[SDL_CONTROLLER_AXIS_LEFTY];
        if (axisName == "RightX") return m_axes[SDL_CONTROLLER_AXIS_RIGHTX];
        if (axisName == "RightY") return m_axes[SDL_CONTROLLER_AXIS_RIGHTY];
        if (axisName == "TriggerLeft") return m_axes[SDL_CONTROLLER_AXIS_TRIGGERLEFT];
        if (axisName == "TriggerRight") return m_axes[SDL_CONTROLLER_AXIS_TRIGGERRIGHT];
        return 0.0f;
    }

    bool InputSystem::IsGamepadButtonPressed(const std::string& buttonName) const {
        if (!m_gamepad) return false;
        if (buttonName == "A") return m_buttons[SDL_CONTROLLER_BUTTON_A];
        if (buttonName == "B") return m_buttons[SDL_CONTROLLER_BUTTON_B];
        if (buttonName == "X") return m_buttons[SDL_CONTROLLER_BUTTON_X];
        if (buttonName == "Y") return m_buttons[SDL_CONTROLLER_BUTTON_Y];
        if (buttonName == "Start") return m_buttons[SDL_CONTROLLER_BUTTON_START];
        if (buttonName == "Back") return m_buttons[SDL_CONTROLLER_BUTTON_BACK];
        return false;
    }

    void InputSystem::Vibrate(float leftMotor, float rightMotor, uint32_t durationMS) {
        if (m_gamepad) {
            SDL_GameControllerRumble((SDL_GameController*)m_gamepad, (uint16_t)(leftMotor * 0xFFFF), (uint16_t)(rightMotor * 0xFFFF), durationMS);
        }
    }

}
