#pragma once
#include <functional>
#include <memory>
#include <string>
#include <cstdint>

struct SDL_Window;

namespace starlight::platform {

enum class WindowMode {
    Windowed,
    BorderlessFullscreen,
    ExclusiveFullscreen
};

enum class CursorMode {
    Normal,
    Hidden,
    Disabled // Relative/captured mouse mode for first/third person cameras
};

struct WindowProps {
    std::string title = "Fusion ENGINE";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool vsync = true;
    bool resizable = true;
    bool highDPI = true;
    WindowMode mode = WindowMode::Windowed;
    CursorMode cursor = CursorMode::Normal;
    int maxFPS = 0; // 0 = unlimited
};

/**
 * @brief Multi-platform Window abstraction interface.
 */
class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool Initialize(const WindowProps& props) = 0;
    virtual void Shutdown() = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;

    virtual bool ShouldClose() const = 0;
    virtual void SetShouldClose(bool close) = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual float GetAspectRatio() const {
        return GetHeight() > 0 ? static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()) : 1.0f;
    }

    virtual void SetTitle(const std::string& title) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    virtual void SetWindowMode(WindowMode mode) = 0;
    virtual WindowMode GetWindowMode() const = 0;

    virtual void SetCursorMode(CursorMode mode) = 0;
    virtual CursorMode GetCursorMode() const = 0;

    virtual void* GetNativeWindowHandle() const = 0;
    virtual void* GetGraphicsContext() const = 0;

    using EventCallbackFn = std::function<void(class Event&)>;
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

    static std::unique_ptr<IWindow> Create(const WindowProps& props = WindowProps{});
};

} // namespace starlight::platform
