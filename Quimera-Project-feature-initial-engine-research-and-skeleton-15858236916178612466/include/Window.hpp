// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <functional>

namespace Vamos {
    class Window {
    public:
        using KeyCallback = std::function<void(int, int, int, int)>;

        Window(int width, int height, const std::string& title) {
            if (!glfwInit()) {
                std::cerr << "Falha ao inicializar GLFW" << std::endl;
                return;
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
            if (!window) {
                std::cerr << "Falha ao criar janela GLFW" << std::endl;
                glfwTerminate();
                return;
            }

            glfwMakeContextCurrent(window); // ESSENCIAL PARA GLEW FUNCIONAR

            glfwSetWindowUserPointer(window, this);

            glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
                auto self = static_cast<Window*>(glfwGetWindowUserPointer(win));
                if (self->keyCallback) {
                    self->keyCallback(key, scancode, action, mods);
                }
            });
        }

        ~Window() {
            if (window) {
                glfwDestroyWindow(window);
            }
            glfwTerminate();
        }

        bool ShouldClose() const {
            return window ? glfwWindowShouldClose(window) : true;
        }

        void PollEvents() {
            glfwPollEvents();
        }

        void SwapBuffers() {
            if (window) glfwSwapBuffers(window);
        }

        void SetKeyCallback(KeyCallback callback) {
            keyCallback = callback;
        }

        GLFWwindow* GetNativeWindow() const { return window; }

    private:
        GLFWwindow* window;
        KeyCallback keyCallback;
    };
}
