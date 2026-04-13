#include <unordered_map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

namespace titan {
    struct InputAction {
        std::string name;
        std::vector<SDL_Scancode> keys;
        std::vector<uint8_t> mouseButtons;
    };

    class InputSystem {
    public:
        void Initialize();
        void Update();
        
        // Semantic Action Checks
        bool IsActionPressed(const std::string& name) const;
        bool IsActionJustPressed(const std::string& name) const;
        
        static bool IsKeyPressed(SDL_Scancode key);
        static bool IsKeyJustPressed(SDL_Scancode key);

        void BindAction(const std::string& name, SDL_Scancode key);
        void BindMouseButton(const std::string& name, uint8_t button);

        glm::vec2 GetMousePosition() const { return m_mousePos; }

    private:
        std::unordered_map<std::string, InputAction> m_actions;
        static uint8_t s_currKeys[SDL_NUM_SCANCODES];
        static uint8_t s_prevKeys[SDL_NUM_SCANCODES];
        glm::vec2 m_mousePos;
    };
}
