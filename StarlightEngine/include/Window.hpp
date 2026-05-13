// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>

// Forward declarations
struct SDL_Window;
typedef void* SDL_GLContext;

namespace starlight {
    struct WindowConfig {
        std::string title = "Titan Engine";
        int width = 1280;
        int height = 720;
        bool vsync = true;
        bool resizable = true;
    };

    class Window {
    public:
        Window();
        ~Window();

        void Initialize(const WindowConfig& config);
        void Shutdown();

        bool ShouldClose() const { return m_shouldClose; }
        void PollEvents();
        void SwapBuffers();

        SDL_Window* GetNativeWindow() const { return m_window; }
        void* GetGLContext() const { return m_glContext; }

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        SDL_Window* m_window = nullptr;
        void* m_glContext = nullptr;
        int m_width, m_height;
        bool m_shouldClose = false;
    };
}
