// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>

namespace starlight {

    class Window;
    class Engine;

    struct EngineContext {
        Window* window = nullptr;
        Engine* engine = nullptr;
    };

    class ISystem {
    public:
        virtual ~ISystem() = default;
        
        virtual bool OnInitialize(const EngineContext& context) = 0;
        virtual void OnShutdown() = 0;
        
        virtual void OnUpdate(float dt) { (void)dt; }
        virtual void OnFixedUpdate(float dt) { (void)dt; }
        virtual void OnRender() {}
        virtual void OnUIRender() {}
        
        virtual const char* GetName() const = 0;
    };

    class SystemRegistry {
    public:
        template<typename T, typename... Args>
        T* RegisterSystem(Args&&... args) {
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            m_systems.push_back(system);
            m_systemMap[typeid(T)] = system;
            return system.get();
        }

        template<typename T>
        T* GetSystem() {
            auto it = m_systemMap.find(typeid(T));
            if (it != m_systemMap.end()) {
                return static_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        const std::vector<std::shared_ptr<ISystem>>& GetSystems() const {
            return m_systems;
        }

        void Shutdown() {
            // Shutdown in reverse order
            for (auto it = m_systems.rbegin(); it != m_systems.rend(); ++it) {
                (*it)->OnShutdown();
            }
            m_systems.clear();
            m_systemMap.clear();
        }

    private:
        std::vector<std::shared_ptr<ISystem>> m_systems;
        std::unordered_map<std::type_index, std::shared_ptr<ISystem>> m_systemMap;
    };

}
