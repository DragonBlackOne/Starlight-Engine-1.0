// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "InputSystem.hpp"
#include <cstring>
#include <SDL2/SDL.h>

namespace titan {

    uint8_t InputSystem::s_currKeys[SDL_NUM_SCANCODES] = {0};
    uint8_t InputSystem::s_prevKeys[SDL_NUM_SCANCODES] = {0};

    void InputSystem::Initialize() {
        std::memset(s_currKeys, 0, SDL_NUM_SCANCODES);
        std::memset(s_prevKeys, 0, SDL_NUM_SCANCODES);

        // Professional Default Bindings
        BindAction("Jump", SDL_SCANCODE_SPACE);
        BindAction("MoveForward", SDL_SCANCODE_W);
        BindAction("MoveBackward", SDL_SCANCODE_S);
        BindAction("MoveLeft", SDL_SCANCODE_A);
        BindAction("MoveRight", SDL_SCANCODE_D);
        BindAction("EditorToggle", SDL_SCANCODE_F2);
    }

    void InputSystem::Update() {
        std::memcpy(s_prevKeys, s_currKeys, SDL_NUM_SCANCODES);
        const uint8_t* state = SDL_GetKeyboardState(NULL);
        std::memcpy(s_currKeys, state, SDL_NUM_SCANCODES);

        int mx, my;
        SDL_GetMouseState(&mx, &my);
        m_mousePos = glm::vec2(mx, my);
    }

    bool InputSystem::IsActionPressed(const std::string& name) const {
        auto it = m_actions.find(name);
        if (it != m_actions.end()) {
            for (auto key : it->second.keys) {
                if (IsKeyPressed(key)) return true;
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
        }
        return false;
    }

    bool InputSystem::IsKeyPressed(SDL_Scancode key) {
        return s_currKeys[key] != 0;
    }

    bool InputSystem::IsKeyJustPressed(SDL_Scancode key) {
        return (s_currKeys[key] != 0) && (s_prevKeys[key] == 0);
    }

    void InputSystem::BindAction(const std::string& name, SDL_Scancode key) {
        m_actions[name].name = name;
        m_actions[name].keys.push_back(key);
    }

    void InputSystem::BindMouseButton(const std::string& name, uint8_t button) {
        m_actions[name].name = name;
        m_actions[name].mouseButtons.push_back(button);
    }

}
