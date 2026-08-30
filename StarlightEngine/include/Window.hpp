#pragma once
#include <functional>
#include <string>

// Forward declarations
struct SDL_Window;
typedef void* SDL_GLContext;

namespace starlight {
struct WindowConfig {
    std::string title = "Starlight Engine";
    int width = 1280;
    int height = 720;
    bool vsync = true;
    bool resizable = true;
    bool mode2D = false;
    int maxFPS = 0;  // 0 = unlimited
    bool fullscreen = false;
};

class Window {
public:
    Window();
    ~Window();

    bool Initialize(const WindowConfig& config);
    void Shutdown();

    bool ShouldClose() const {
        return m_shouldClose;
    }
    void PollEvents();
    void SwapBuffers();

    SDL_Window* GetNativeWindow() const {
        return m_window;
    }
    void* GetGLContext() const {
        return m_glContext;
    }

    int GetWidth() const {
        return m_width;
    }
    int GetHeight() const {
        return m_height;
    }

    float GetScrollDelta() const {
        return m_scrollDelta;
    }

    void SetFullscreen(bool fullscreen);
    void ToggleFullscreen();
    bool IsFullscreen() const;
    void SetTitle(const std::string& title);
    void SetIcon(const std::string& path);
    bool CaptureScreenshot(const std::string& filepath);
    bool IsInitialized() const {
        return m_initialized;
    }

    using ResizeCallback = std::function<void(int, int)>;
    void SetResizeCallback(ResizeCallback cb) {
        m_resizeCallback = cb;
    }

private:
    SDL_Window* m_window = nullptr;
    void* m_glContext = nullptr;
    int m_width = 0;
    int m_height = 0;
    bool m_shouldClose = false;
    bool m_initialized = false;
    float m_scrollDelta = 0.0f;
    ResizeCallback m_resizeCallback;
};
}  // namespace starlight
