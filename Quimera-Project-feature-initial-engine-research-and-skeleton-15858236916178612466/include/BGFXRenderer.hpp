// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - BGFX Renderer Abstraction
// Licença: BSD 2-Clause | Origem: bkaradzic/bgfx
// ============================================================

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#if BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include "Renderer.hpp"
#include "Log.hpp"

namespace Vamos {
    class BGFXRenderer : public IRenderer {
    public:
        BGFXRenderer(GLFWwindow* _window, int _width, int _height) 
            : window(_window), width(_width), height(_height) {}

        bool Initialize() override {
            bgfx::renderFrame();

            bgfx::Init init;
            init.type = bgfx::RendererType::Count; // Auto-select (Vulkan, DX12, etc.)
            init.resolution.width = width;
            init.resolution.height = height;
            init.resolution.reset = BGFX_RESET_VSYNC;

#if BX_PLATFORM_WINDOWS
            init.platformData.nwh = glfwGetWin32Window(window);
#elif BX_PLATFORM_LINUX
            init.platformData.ndt = glfwGetX11Display();
            init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#endif
            if (!bgfx::init(init)) {
                VAMOS_CORE_ERROR("[BGFXRenderer] Falha ao inicializar BGFX!");
                return false;
            }

            VAMOS_CORE_INFO("[BGFXRenderer] BGFX Inicializado (API: {0})", bgfx::getRendererName(bgfx::getRendererType()));

            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
            bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

            return true;
        }

        void Shutdown() override {
            bgfx::shutdown();
            VAMOS_CORE_INFO("[BGFXRenderer] BGFX Shutdown completo");
        }

        void Render(const RenderView& view) override {
            bgfx::touch(0);

            // Setar View & Projection matrices (BGFX espera column-major float ptrs)
            bgfx::setViewTransform(0, &view.viewMatrix[0][0], &view.projectionMatrix[0][0]);

            // Rendering simulado por enquanto (bgfx usa state machines para meshes)
            // Aqui você acopla os draw calls da engine ao bgfx::submit(0, program);

            bgfx::frame();
        }

        void Resize(int newWidth, int newHeight) {
            width = newWidth;
            height = newHeight;
            bgfx::reset(width, height, BGFX_RESET_VSYNC);
            bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        }

    private:
        GLFWwindow* window;
        int width, height;
    };
}
