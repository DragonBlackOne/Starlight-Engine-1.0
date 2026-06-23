#include "InputSystem.hpp"
#include "ConfigSystem.hpp"
#include "Engine.hpp"
#include <cstring>
#include <SDL2/SDL.h>
#include <algorithm>
#include <unordered_map>

namespace starlight {

    static SDL_Scancode ToSDLScancode(pal::KeyCode key) {
        return static_cast<SDL_Scancode>(key);
    }

    pal::KeyCode InputSystem::KeyCodeFromString(const std::string& name) {
        static const std::unordered_map<std::string, pal::KeyCode> s_stringToKey = {
            {"A", pal::KeyCode::A}, {"B", pal::KeyCode::B}, {"C", pal::KeyCode::C}, {"D", pal::KeyCode::D},
            {"E", pal::KeyCode::E}, {"F", pal::KeyCode::F}, {"G", pal::KeyCode::G}, {"H", pal::KeyCode::H},
            {"I", pal::KeyCode::I}, {"J", pal::KeyCode::J}, {"K", pal::KeyCode::K}, {"L", pal::KeyCode::L},
            {"M", pal::KeyCode::M}, {"N", pal::KeyCode::N}, {"O", pal::KeyCode::O}, {"P", pal::KeyCode::P},
            {"Q", pal::KeyCode::Q}, {"R", pal::KeyCode::R}, {"S", pal::KeyCode::S}, {"T", pal::KeyCode::T},
            {"U", pal::KeyCode::U}, {"V", pal::KeyCode::V}, {"W", pal::KeyCode::W}, {"X", pal::KeyCode::X},
            {"Y", pal::KeyCode::Y}, {"Z", pal::KeyCode::Z},
            {"1", pal::KeyCode::Num1}, {"2", pal::KeyCode::Num2}, {"3", pal::KeyCode::Num3}, {"4", pal::KeyCode::Num4},
            {"5", pal::KeyCode::Num5}, {"6", pal::KeyCode::Num6}, {"7", pal::KeyCode::Num7}, {"8", pal::KeyCode::Num8},
            {"9", pal::KeyCode::Num9}, {"0", pal::KeyCode::Num0},
            {"Return", pal::KeyCode::Return}, {"Enter", pal::KeyCode::Return},
            {"Escape", pal::KeyCode::Escape}, {"Backspace", pal::KeyCode::Backspace},
            {"Tab", pal::KeyCode::Tab}, {"Space", pal::KeyCode::Space},
            {"F1", pal::KeyCode::F1}, {"F2", pal::KeyCode::F2}, {"F3", pal::KeyCode::F3}, {"F4", pal::KeyCode::F4},
            {"F5", pal::KeyCode::F5}, {"F6", pal::KeyCode::F6}, {"F7", pal::KeyCode::F7}, {"F8", pal::KeyCode::F8},
            {"F9", pal::KeyCode::F9}, {"F10", pal::KeyCode::F10}, {"F11", pal::KeyCode::F11}, {"F12", pal::KeyCode::F12},
            {"PrintScreen", pal::KeyCode::PrintScreen}, {"ScrollLock", pal::KeyCode::ScrollLock}, {"Pause", pal::KeyCode::Pause},
            {"Insert", pal::KeyCode::Insert}, {"Home", pal::KeyCode::Home}, {"PageUp", pal::KeyCode::PageUp},
            {"Delete", pal::KeyCode::Delete}, {"End", pal::KeyCode::End}, {"PageDown", pal::KeyCode::PageDown},
            {"Right", pal::KeyCode::Right}, {"Left", pal::KeyCode::Left}, {"Down", pal::KeyCode::Down}, {"Up", pal::KeyCode::Up},
            {"LShift", pal::KeyCode::LShift}, {"LCtrl", pal::KeyCode::LCtrl}, {"LAlt", pal::KeyCode::LAlt},
            {"RShift", pal::KeyCode::RShift}, {"RCtrl", pal::KeyCode::RCtrl}, {"RAlt", pal::KeyCode::RAlt}
        };
        auto it = s_stringToKey.find(name);
        if (it != s_stringToKey.end()) return it->second;
        return pal::KeyCode::Unknown;
    }

    std::string InputSystem::StringFromKeyCode(pal::KeyCode key) {
        static const std::unordered_map<pal::KeyCode, std::string> s_keyToString = {
            {pal::KeyCode::A, "A"}, {pal::KeyCode::B, "B"}, {pal::KeyCode::C, "C"}, {pal::KeyCode::D, "D"},
            {pal::KeyCode::E, "E"}, {pal::KeyCode::F, "F"}, {pal::KeyCode::G, "G"}, {pal::KeyCode::H, "H"},
            {pal::KeyCode::I, "I"}, {pal::KeyCode::J, "J"}, {pal::KeyCode::K, "K"}, {pal::KeyCode::L, "L"},
            {pal::KeyCode::M, "M"}, {pal::KeyCode::N, "N"}, {pal::KeyCode::O, "O"}, {pal::KeyCode::P, "P"},
            {pal::KeyCode::Q, "Q"}, {pal::KeyCode::R, "R"}, {pal::KeyCode::S, "S"}, {pal::KeyCode::T, "T"},
            {pal::KeyCode::U, "U"}, {pal::KeyCode::V, "V"}, {pal::KeyCode::W, "W"}, {pal::KeyCode::X, "X"},
            {pal::KeyCode::Y, "Y"}, {pal::KeyCode::Z, "Z"},
            {pal::KeyCode::Num1, "1"}, {pal::KeyCode::Num2, "2"}, {pal::KeyCode::Num3, "3"}, {pal::KeyCode::Num4, "4"},
            {pal::KeyCode::Num5, "5"}, {pal::KeyCode::Num6, "6"}, {pal::KeyCode::Num7, "7"}, {pal::KeyCode::Num8, "8"},
            {pal::KeyCode::Num9, "9"}, {pal::KeyCode::Num0, "0"},
            {pal::KeyCode::Return, "Return"}, {pal::KeyCode::Escape, "Escape"}, {pal::KeyCode::Backspace, "Backspace"},
            {pal::KeyCode::Tab, "Tab"}, {pal::KeyCode::Space, "Space"},
            {pal::KeyCode::F1, "F1"}, {pal::KeyCode::F2, "F2"}, {pal::KeyCode::F3, "F3"}, {pal::KeyCode::F4, "F4"},
            {pal::KeyCode::F5, "F5"}, {pal::KeyCode::F6, "F6"}, {pal::KeyCode::F7, "F7"}, {pal::KeyCode::F8, "F8"},
            {pal::KeyCode::F9, "F9"}, {pal::KeyCode::F10, "F10"}, {pal::KeyCode::F11, "F11"}, {pal::KeyCode::F12, "F12"},
            {pal::KeyCode::PrintScreen, "PrintScreen"}, {pal::KeyCode::ScrollLock, "ScrollLock"}, {pal::KeyCode::Pause, "Pause"},
            {pal::KeyCode::Insert, "Insert"}, {pal::KeyCode::Home, "Home"}, {pal::KeyCode::PageUp, "PageUp"},
            {pal::KeyCode::Delete, "Delete"}, {pal::KeyCode::End, "End"}, {pal::KeyCode::PageDown, "PageDown"},
            {pal::KeyCode::Right, "Right"}, {pal::KeyCode::Left, "Left"}, {pal::KeyCode::Down, "Down"}, {pal::KeyCode::Up, "Up"},
            {pal::KeyCode::LShift, "LShift"}, {pal::KeyCode::LCtrl, "LCtrl"}, {pal::KeyCode::LAlt, "LAlt"},
            {pal::KeyCode::RShift, "RShift"}, {pal::KeyCode::RCtrl, "RCtrl"}, {pal::KeyCode::RAlt, "RAlt"}
        };
        auto it = s_keyToString.find(key);
        if (it != s_keyToString.end()) return it->second;
        return "Unknown";
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
        BindAction("Enter", pal::KeyCode::Return);
        BindAction("Return", pal::KeyCode::Return);
        
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

        // Load custom bindings from config
        auto configSys = Engine::Get().GetSystem<ConfigSystem>();
        if (configSys) {
            for (auto& [actionName, action] : m_actions) {
                std::string currentKeyStr = "";
                if (!action.keys.empty()) {
                    currentKeyStr = StringFromKeyCode(action.keys[0]);
                }
                
                std::string newKeyStr = configSys->GetString("Input", actionName, currentKeyStr);
                if (newKeyStr != currentKeyStr && !newKeyStr.empty()) {
                    pal::KeyCode newKey = KeyCodeFromString(newKeyStr);
                    if (newKey != pal::KeyCode::Unknown) {
                        action.keys.clear();
                        action.keys.push_back(newKey);
                    }
                }
            }
        }

        return true;
    }

    void InputSystem::OnUpdate(float dt) {
        (void)dt;
        std::memcpy(s_prevKeys, s_currKeys, 512);
        int numkeys = 0;
        const uint8_t* state = SDL_GetKeyboardState(&numkeys);
        std::memset(s_currKeys, 0, 512);
        std::memcpy(s_currKeys, state, std::min(numkeys, 512));

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

    void InputSystem::SaveBindings(ConfigSystem& config) const {
        for (const auto& [actionName, action] : m_actions) {
            if (!action.keys.empty()) {
                config.SetString("Input", actionName, StringFromKeyCode(action.keys[0]));
            }
        }
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

    void InputSystem::OnShutdown() {
        auto configSys = Engine::Get().GetSystem<ConfigSystem>();
        if (configSys) {
            SaveBindings(*configSys);
        }

        if (m_gamepad) {
            SDL_GameControllerClose((SDL_GameController*)m_gamepad);
            m_gamepad = nullptr;
        }
    }

}
