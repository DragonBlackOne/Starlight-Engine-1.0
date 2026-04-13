#pragma once
// ============================================================
// Vamos Engine - UI Middleware (RmlUi)
// Licença: MIT | Origem: mikke89/RmlUi
// ============================================================

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <string>
#include "Log.hpp"

namespace Vamos {
    // É necessário ter o Renderer (BGFX/OpenGL) e um FileInterface integrados
    // Este wrapper apenas expõe o contexto inicial da RmlUi.
    
    class SystemInterface_Rml : public Rml::SystemInterface {
    public:
        double GetElapsedTime() override {
            // Em uma engine final, retorna glfwGetTime()
            return 0.0; 
        }
        
        bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
            switch (type) {
                case Rml::Log::LT_ERROR:   VAMOS_CORE_ERROR("[RmlUi] {0}", message); break;
                case Rml::Log::LT_ASSERT:  VAMOS_CORE_CRITICAL("[RmlUi] {0}", message); break;
                case Rml::Log::LT_WARNING: VAMOS_CORE_WARN("[RmlUi] {0}", message); break;
                case Rml::Log::LT_INFO:    VAMOS_CORE_INFO("[RmlUi] {0}", message); break;
                default:                   VAMOS_CORE_TRACE("[RmlUi] {0}", message); break;
            }
            return true;
        }
    };

    class UserInterface {
    public:
        UserInterface(Rml::RenderInterface* renderInterface, Rml::FileInterface* fileInterface) {
            systemInterface = new SystemInterface_Rml();
            Rml::SetSystemInterface(systemInterface);
            Rml::SetRenderInterface(renderInterface);
            if (fileInterface) Rml::SetFileInterface(fileInterface);
            
            if (!Rml::Initialise()) {
                VAMOS_CORE_ERROR("[RmlUi] Falha ao inicializar o motor HTML/CSS");
                return;
            }
            Rml::Debugger::Initialise(context);
            VAMOS_CORE_INFO("[UserInterface] RmlUi HTML/CSS Motor ativado.");
        }

        ~UserInterface() {
            Rml::Shutdown();
            delete systemInterface;
        }

        Rml::Context* CreateContext(const std::string& name, int width, int height) {
            context = Rml::CreateContext(name, Rml::Vector2i(width, height));
            return context;
        }

        void LoadDocument(const std::string& path) {
            if (!context) return;
            Rml::ElementDocument* document = context->LoadDocument(path);
            if (document) document->Show();
        }

        void Update() {
            if (context) context->Update();
        }

        void Render() {
            if (context) context->Render();
        }

    private:
        SystemInterface_Rml* systemInterface = nullptr;
        Rml::Context* context = nullptr;
    };
}
