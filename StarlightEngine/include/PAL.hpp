#pragma once
#include <cstdint>

namespace starlight::pal {

    enum class KeyCode : uint32_t {
        Unknown = 0,
        A = 4, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
        Return, Escape, Backspace, Tab, Space,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        PrintScreen, ScrollLock, Pause, Insert, Home, PageUp, Delete, End, PageDown,
        Right, Left, Down, Up,
        LShift, LCtrl, LAlt, RShift, RCtrl, RAlt,
        // Add more as needed based on SDL_Scancode mapping
    };

    enum class MouseButton : uint8_t {
        Left = 1,
        Middle = 2,
        Right = 3,
        X1 = 4,
        X2 = 5
    };

}
