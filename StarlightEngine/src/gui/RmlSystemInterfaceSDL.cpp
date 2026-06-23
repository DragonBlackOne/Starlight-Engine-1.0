#include "RmlSystemInterfaceSDL.hpp"
#include <SDL2/SDL.h>
#include "Log.hpp"

namespace starlight {

    RmlSystemInterfaceSDL::RmlSystemInterfaceSDL() {}
    RmlSystemInterfaceSDL::~RmlSystemInterfaceSDL() {}

    double RmlSystemInterfaceSDL::GetElapsedTime() {
        return (double)SDL_GetTicks() / 1000.0;
    }

    bool RmlSystemInterfaceSDL::LogMessage(Rml::Log::Type type, const Rml::String& message) {
        switch (type) {
            case Rml::Log::LT_INFO:
                Log::Info("[RmlUi] {}", message.c_str());
                break;
            case Rml::Log::LT_WARNING:
                Log::Warn("[RmlUi] {}", message.c_str());
                break;
            case Rml::Log::LT_ERROR:
            case Rml::Log::LT_ASSERT:
                Log::Error("[RmlUi] {}", message.c_str());
                break;
            default:
                break;
        }
        return true;
    }

}
