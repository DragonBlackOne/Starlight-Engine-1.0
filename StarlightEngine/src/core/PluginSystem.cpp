#include "PluginSystem.hpp"
#include "Log.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>

namespace starlight {

    PluginSystem::PluginSystem() {
        Log::Info("PluginSystem created.");
    }

    PluginSystem::~PluginSystem() {
        UnloadPlugin();
    }

    bool PluginSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        Log::Info("PluginSystem initialized.");
        return true;
    }

    void PluginSystem::OnShutdown() {
        UnloadPlugin();
    }

    void PluginSystem::OnUpdate(float dt) {
        if (m_plugin) {
            m_plugin->OnUpdate(dt);
        }
    }

    bool PluginSystem::LoadPlugin(const std::string& path) {
        UnloadPlugin();

        if (!std::filesystem::exists(path)) {
            Log::Error("PluginSystem: File does not exist: {}", path);
            return false;
        }

        m_pluginPath = path;
        m_tempPath = std::filesystem::path(path).parent_path().string() + "/temp_" + std::filesystem::path(path).filename().string();

        try {
            // Copy DLL to temp path to avoid file locking, enabling hot-reload while the app runs
            std::filesystem::copy_file(path, m_tempPath, std::filesystem::copy_options::overwrite_existing);
        }
        catch (const std::exception& e) {
            Log::Error("PluginSystem: Failed to copy plugin to temp path: {}", e.what());
            return false;
        }

        HMODULE handle = LoadLibraryA(m_tempPath.c_str());
        if (!handle) {
            Log::Error("PluginSystem: Failed to load DLL from {}. Error code: {}", m_tempPath, GetLastError());
            std::filesystem::remove(m_tempPath);
            return false;
        }

        m_module = handle;

        m_createFunc = (CreatePluginFunc)GetProcAddress(handle, "CreatePlugin");
        m_destroyFunc = (DestroyPluginFunc)GetProcAddress(handle, "DestroyPlugin");

        if (!m_createFunc || !m_destroyFunc) {
            Log::Error("PluginSystem: DLL does not export CreatePlugin/DestroyPlugin functions.");
            FreeLibrary(handle);
            m_module = nullptr;
            std::filesystem::remove(m_tempPath);
            return false;
        }

        m_plugin = m_createFunc();
        if (!m_plugin) {
            Log::Error("PluginSystem: CreatePlugin returned nullptr.");
            FreeLibrary(handle);
            m_module = nullptr;
            std::filesystem::remove(m_tempPath);
            return false;
        }

        m_plugin->OnLoad();
        Log::Info("PluginSystem: Loaded plugin successfully from {}", path);
        return true;
    }

    void PluginSystem::UnloadPlugin() {
        if (m_plugin) {
            m_plugin->OnUnload();
            if (m_destroyFunc) {
                m_destroyFunc(m_plugin);
            }
            m_plugin = nullptr;
        }

        if (m_module) {
            FreeLibrary((HMODULE)m_module);
            m_module = nullptr;
        }

        m_createFunc = nullptr;
        m_destroyFunc = nullptr;

        if (!m_tempPath.empty() && std::filesystem::exists(m_tempPath)) {
            std::error_code ec;
            std::filesystem::remove(m_tempPath, ec);
            m_tempPath.clear();
        }
    }

    void PluginSystem::HotReload() {
        if (m_pluginPath.empty()) {
            Log::Warn("PluginSystem: No plugin is currently loaded to hot reload.");
            return;
        }

        Log::Info("PluginSystem: Hot-reloading plugin {}...", m_pluginPath);
        std::string cachedPath = m_pluginPath;
        UnloadPlugin();
        
        // Give a tiny delay for resources to release
        Sleep(100);

        if (!LoadPlugin(cachedPath)) {
            Log::Error("PluginSystem: Hot-reload failed!");
        } else {
            Log::Info("PluginSystem: Hot-reload successful!");
        }
    }

}
