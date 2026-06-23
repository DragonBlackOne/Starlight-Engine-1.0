#pragma once
#include <string>
#include <filesystem>
#include <map>
#include <functional>
#include "EngineSystem.hpp"

namespace starlight {

    class FileWatcher : public ISystem {
    public:
        using OnChangedCallback = std::function<void(const std::string&)>;

        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override { (void)dt; Update(); }
        const char* GetName() const override { return "FileWatcher"; }

        void AddWatch(const std::string& path, OnChangedCallback callback);
        void Update();

    private:
        struct WatchInfo {
            std::string path;
            std::filesystem::file_time_type lastWriteTime;
            OnChangedCallback callback;
        };

        std::map<std::string, WatchInfo> m_watches;
    };

}
