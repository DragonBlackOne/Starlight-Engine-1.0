#pragma once
#include <string>
#include <memory>
#include "EngineSystem.hpp"

namespace starlight {

    class IPlugin {
    public:
        virtual ~IPlugin() = default;
        virtual void OnLoad() = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnUnload() = 0;
    };

    typedef IPlugin* (*CreatePluginFunc)();
    typedef void (*DestroyPluginFunc)(IPlugin*);

    class PluginSystem : public ISystem {
    public:
        PluginSystem();
        ~PluginSystem();

        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "PluginSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        bool LoadPlugin(const std::string& path);
        void UnloadPlugin();
        void HotReload();

        bool IsPluginLoaded() const { return m_plugin != nullptr; }

    private:
        std::string m_pluginPath;
        std::string m_tempPath;
        void* m_module = nullptr; // HINSTANCE/HMODULE
        IPlugin* m_plugin = nullptr;
        CreatePluginFunc m_createFunc = nullptr;
        DestroyPluginFunc m_destroyFunc = nullptr;
    };

}
