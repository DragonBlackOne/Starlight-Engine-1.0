#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

// NO GLEW. ONLY GLFW.
#include <GLFW/glfw3.h>

namespace Vamos {
    class Engine {
    public:
        Engine() : running(true) {}

        void Initialize() {
            std::cout << "[INIT] 1. Creating Window (GLFW ONLY)..." << std::endl; std::cout.flush();
            if (!glfwInit()) {
                 std::cout << "GLFW INIT FAILED" << std::endl; std::cout.flush();
                 return;
            }
            GLFWwindow* window = glfwCreateWindow(1280, 720, "Test GLFW ONLY", NULL, NULL);
            if (!window) {
                 std::cout << "WINDOW FAILED" << std::endl; std::cout.flush();
                 return;
            }
            std::cout << "[INIT] 2. Window Created Success" << std::endl; std::cout.flush();
            
            glfwDestroyWindow(window);
            glfwTerminate();
            std::cout << "[INIT] 3. Success Termination" << std::endl; std::cout.flush();
        }

        void Shutdown() {}
        void Run() { running = false; }

    private:
        bool running;
    };
}
