// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "Window.hpp"
#include "Log.hpp"
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"

namespace starlight {
    Window::Window() : m_width(0), m_height(0), m_window(nullptr), m_glContext(nullptr), m_shouldClose(false) {}

    Window::~Window() {
        Shutdown();
    }

    void Window::Initialize(const WindowConfig& config) {
        m_width = config.width;
        m_height = config.height;

        #ifdef SDL_MAIN_HANDLED
        SDL_SetMainReady();
        #endif

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) < 0) {
            Log::Error("Failed to initialize SDL2: {}", SDL_GetError());
            return;
        }

        // OpenGL Attributes (4.5 Core)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
        if (config.resizable) flags |= SDL_WINDOW_RESIZABLE;

        m_window = SDL_CreateWindow(
            config.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_width,
            m_height,
            flags
        );

        if (!m_window) {
            Log::Error("Failed to create SDL2 Window: {}", SDL_GetError());
            return;
        }

        m_glContext = SDL_GL_CreateContext(m_window);
        if (!m_glContext) {
            Log::Error("Failed to create OpenGL context: {}", SDL_GetError());
            return;
        }

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            Log::Error("Failed to initialize GLAD");
            return;
        }

        SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);
        Log::Info("Window initialized: {} ({}x{})", config.title, m_width, m_height);
        Log::Info("OpenGL Renderer: {}", (const char*)glGetString(GL_RENDERER));
    }

    void Window::Shutdown() {
        if (m_glContext) {
            SDL_GL_DeleteContext(m_glContext);
            m_glContext = nullptr;
        }
        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        SDL_Quit();
    }

    void Window::PollEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            if (event.type == SDL_QUIT) {
                m_shouldClose = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                m_width = event.window.data1;
                m_height = event.window.data2;
                glViewport(0, 0, m_width, m_height);
            }
        }
    }

    void Window::SwapBuffers() {
        if (m_window) {
            SDL_GL_SwapWindow(m_window);
        }
    }
}
