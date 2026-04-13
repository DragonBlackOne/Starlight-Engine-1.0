#include "FileWatcher.hpp"
#include "Log.hpp"

namespace titan {

    void FileWatcher::AddWatch(const std::string& path, OnChangedCallback callback) {
        if (!std::filesystem::exists(path)) {
            Log::Error("FileWatcher: Cannot watch non-existent path: " + path);
            return;
        }

        m_watches[path] = { path, std::filesystem::last_write_time(path), callback };
    }

    void FileWatcher::Update() {
        for (auto& [path, info] : m_watches) {
            if (!std::filesystem::exists(path)) continue;

            auto currentTime = std::filesystem::last_write_time(path);
            if (currentTime != info.lastWriteTime) {
                info.lastWriteTime = currentTime;
                Log::Info("FileWatcher: Change detected in " + path);
                info.callback(path);
            }
        }
    }

}
