#pragma once
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <sol/sol.hpp>
#pragma warning(pop)
#include <memory>
#include <string>

#include "EngineSystem.hpp"

namespace starlight {
    class ScriptSystem : public ISystem {
    public:
        ScriptSystem();
        ~ScriptSystem();

        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override {}
        void OnUpdate(float dt) override;
        void OnRender() override;
        void OnUIRender() override;
        const char* GetName() const override { return "ScriptSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        void ExecuteFile(const std::string& path);
        void ResetState();
        
        sol::state& GetLua() { return m_lua; }

    private:
        sol::state m_lua;
        bool m_coreLoaded = false;
        bool m_bridgeLoaded = false;
    };
}
