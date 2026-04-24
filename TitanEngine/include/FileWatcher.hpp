// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <filesystem>
#include <map>
#include <functional>

namespace titan {

    class FileWatcher {
    public:
        using OnChangedCallback = std::function<void(const std::string&)>;

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
