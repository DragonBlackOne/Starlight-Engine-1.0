// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Input System Module (Gainput)
// Licença: MIT | Origem: jkuhlmann/gainput
// ============================================================

#include <gainput/gainput.h>
#include <memory>
#include "Log.hpp"

namespace Vamos {
    // Ações de input mapeáveis
    enum class InputAction : unsigned int {
        // Movimento
        MoveForward = 0,
        MoveBackward,
        MoveLeft,
        MoveRight,
        Jump,
        Crouch,

        // Câmera
        LookX,
        LookY,

        // Ações
        Fire,
        AltFire,
        Interact,
        Reload,
        Pause,
        Sprint,

        // UI
        UIConfirm,
        UICancel,
        UIUp,
        UIDown,
        UILeft,
        UIRight,

        COUNT
    };

    class InputSystem {
    public:
        InputSystem(int windowWidth, int windowHeight)
            : manager(windowWidth, windowHeight)
        {
            // Criar dispositivos
            keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();
            mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
            padId = manager.CreateDevice<gainput::InputDevicePad>();

            // Criar mapa de input
            map = std::make_unique<gainput::InputMap>(manager);

            // Bindings padrão - Teclado
            map->MapBool(static_cast<unsigned>(InputAction::MoveForward), keyboardId, gainput::KeyW);
            map->MapBool(static_cast<unsigned>(InputAction::MoveBackward), keyboardId, gainput::KeyS);
            map->MapBool(static_cast<unsigned>(InputAction::MoveLeft), keyboardId, gainput::KeyA);
            map->MapBool(static_cast<unsigned>(InputAction::MoveRight), keyboardId, gainput::KeyD);
            map->MapBool(static_cast<unsigned>(InputAction::Jump), keyboardId, gainput::KeySpace);
            map->MapBool(static_cast<unsigned>(InputAction::Sprint), keyboardId, gainput::KeyShiftL);
            map->MapBool(static_cast<unsigned>(InputAction::Interact), keyboardId, gainput::KeyE);
            map->MapBool(static_cast<unsigned>(InputAction::Pause), keyboardId, gainput::KeyEscape);

            // Bindings - Mouse
            map->MapBool(static_cast<unsigned>(InputAction::Fire), mouseId, gainput::MouseButtonLeft);
            map->MapBool(static_cast<unsigned>(InputAction::AltFire), mouseId, gainput::MouseButtonRight);
            map->MapFloat(static_cast<unsigned>(InputAction::LookX), mouseId, gainput::MouseAxisX);
            map->MapFloat(static_cast<unsigned>(InputAction::LookY), mouseId, gainput::MouseAxisY);

            // Bindings - Gamepad
            map->MapBool(static_cast<unsigned>(InputAction::Fire), padId, gainput::PadButtonR1);
            map->MapBool(static_cast<unsigned>(InputAction::Jump), padId, gainput::PadButtonA);
            map->MapFloat(static_cast<unsigned>(InputAction::LookX), padId, gainput::PadButtonRightStickX);
            map->MapFloat(static_cast<unsigned>(InputAction::LookY), padId, gainput::PadButtonRightStickY);

            VAMOS_CORE_INFO("InputSystem: Gainput inicializado (Teclado + Mouse + Gamepad)");
        }

        void Update() {
            manager.Update();
        }

        bool IsPressed(InputAction action) const {
            return map->GetBoolIsNew(static_cast<unsigned>(action));
        }

        bool IsHeld(InputAction action) const {
            return map->GetBool(static_cast<unsigned>(action));
        }

        float GetFloat(InputAction action) const {
            return map->GetFloat(static_cast<unsigned>(action));
        }

        float GetFloatDelta(InputAction action) const {
            return map->GetFloatDelta(static_cast<unsigned>(action));
        }

        // Rebind em runtime
        void MapKey(InputAction action, gainput::Key key) {
            map->MapBool(static_cast<unsigned>(action), keyboardId, key);
        }

        void HandleMessage(const MSG& msg) {
            manager.HandleMessage(msg);
        }

        gainput::InputManager& GetManager() { return manager; }

    private:
        gainput::InputManager manager;
        gainput::DeviceId keyboardId;
        gainput::DeviceId mouseId;
        gainput::DeviceId padId;
        std::unique_ptr<gainput::InputMap> map;
    };
}
