#include "Window.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "Log.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "stb_image.h"
#include "PathResolver.hpp"
#include "Engine.hpp"
#include "EditorSystem.hpp"

namespace starlight {
Window::Window()
    : m_width(0), m_height(0), m_window(nullptr), m_glContext(nullptr), m_shouldClose(false), m_initialized(false) {}

Window::~Window() {
    Shutdown();
}

bool Window::Initialize(const WindowConfig& config) {
    if (m_initialized)
        return true;

    m_width = config.width;
    m_height = config.height;

#ifdef SDL_MAIN_HANDLED
    SDL_SetMainReady();
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) < 0) {
        Log::Error("Failed to initialize SDL2: {}", SDL_GetError());
        return false;
    }

    // OpenGL Attributes (4.6 Core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (config.resizable)
        flags |= SDL_WINDOW_RESIZABLE;
    if (config.fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    m_window = SDL_CreateWindow(
        config.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, flags);

    if (!m_window) {
        Log::Error("Failed to create SDL2 Window: {}", SDL_GetError());
        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        Log::Error("Failed to create OpenGL context: {}", SDL_GetError());
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        Log::Error("Failed to initialize GLAD");
        return false;
    }

    SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);

    // ImGui Initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable default imgui.ini auto-load to prevent directory pollution
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init("#version 460");

    m_initialized = true;

    Log::Info("Window initialized: {} ({}x{})", config.title, m_width, m_height);
    Log::Info("OpenGL Renderer: {}", (const char*)glGetString(GL_RENDERER));
    return true;
}

void Window::Shutdown() {
    if (!m_initialized)
        return;
    m_initialized = false;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

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
    m_scrollDelta = 0.0f;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (Engine::IsInitialized()) {
            auto* editor = Engine::Get().GetSystem<EditorSystem>();
            if (editor) {
                editor->ProcessEvent(&event);
            }
        }

        if (event.type == SDL_QUIT) {
            m_shouldClose = true;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            m_width = event.window.data1;
            m_height = event.window.data2;
            glViewport(0, 0, m_width, m_height);
            if (m_resizeCallback) {
                m_resizeCallback(m_width, m_height);
            }
        }
        if (event.type == SDL_MOUSEWHEEL) {
            m_scrollDelta = event.wheel.preciseY;
        }
    }
}

void Window::SwapBuffers() {
    if (m_window) {
        SDL_GL_SwapWindow(m_window);
    }
}

void Window::SetFullscreen(bool fullscreen) {
    if (m_window) {
        Uint32 flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        SDL_SetWindowFullscreen(m_window, flags);
    }
}

void Window::ToggleFullscreen() {
    if (m_window) {
        Uint32 flags = SDL_GetWindowFlags(m_window);
        bool isFullscreen = (flags & SDL_WINDOW_FULLSCREEN) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
        SetFullscreen(!isFullscreen);
    }
}

bool Window::IsFullscreen() const {
    if (m_window) {
        Uint32 flags = SDL_GetWindowFlags(m_window);
        return (flags & SDL_WINDOW_FULLSCREEN) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    return false;
}

void Window::SetTitle(const std::string& title) {
    if (m_window) {
        SDL_SetWindowTitle(m_window, title.c_str());
    }
}

void Window::SetIcon(const std::string& path) {
    if (!m_window)
        return;
    int width = 0, height = 0, channels = 0;
    std::string resolved = PathResolver::Resolve(path);
    unsigned char* data = stbi_load(resolved.c_str(), &width, &height, &channels, 4);
    if (!data) {
        Log::Warn("Failed to load window icon: {}", path);
        return;
    }

    SDL_Surface* surface =
        SDL_CreateRGBSurfaceWithFormatFrom(data, width, height, 32, width * 4, SDL_PIXELFORMAT_RGBA32);

    if (surface) {
        SDL_SetWindowIcon(m_window, surface);
        SDL_FreeSurface(surface);
    } else {
        Log::Warn("Failed to create surface for window icon: {}", SDL_GetError());
    }
    stbi_image_free(data);
}
}  // namespace starlight
