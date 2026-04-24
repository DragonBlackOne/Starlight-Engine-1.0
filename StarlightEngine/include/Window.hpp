// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <SDL.h>

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
        SDL_GLContext GetGLContext() const { return m_glContext; }

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        SDL_Window* m_window = nullptr;
        SDL_GLContext m_glContext = nullptr;
        int m_width, m_height;
        bool m_shouldClose = false;
    };
}
