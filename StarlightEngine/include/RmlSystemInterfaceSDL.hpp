#pragma once
#include <RmlUi/Core/SystemInterface.h>

namespace starlight {

    class RmlSystemInterfaceSDL : public Rml::SystemInterface {
    public:
        RmlSystemInterfaceSDL();
        virtual ~RmlSystemInterfaceSDL();

        // Rml::SystemInterface Implementation
        double GetElapsedTime() override;
        bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
    };

}
