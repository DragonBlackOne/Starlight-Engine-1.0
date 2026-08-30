#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <mutex>

namespace starlight::assets {

enum class FileAction {
    Modified,
    Added,
    Removed
};

class FileWatcher {
public:
    using FileChangeCallback = std::function<void(const std::string& path, FileAction action)>;

    void Watch(const std::string& filepath, FileChangeCallback callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callbacks[filepath] = callback;

        if (std::filesystem::exists(filepath)) {
            m_lastWriteTimes[filepath] = std::filesystem::last_write_time(filepath);
        }
    }

    void Unwatch(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callbacks.erase(filepath);
        m_lastWriteTimes.erase(filepath);
    }

    void PollChanges() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& [path, callback] : m_callbacks) {
            if (std::filesystem::exists(path)) {
                auto currentWriteTime = std::filesystem::last_write_time(path);
                auto it = m_lastWriteTimes.find(path);
                if (it != m_lastWriteTimes.end()) {
                    if (it->second != currentWriteTime) {
                        it->second = currentWriteTime;
                        callback(path, FileAction::Modified);
                    }
                } else {
                    m_lastWriteTimes[path] = currentWriteTime;
                    callback(path, FileAction::Added);
                }
            } else {
                if (m_lastWriteTimes.find(path) != m_lastWriteTimes.end()) {
                    m_lastWriteTimes.erase(path);
                    callback(path, FileAction::Removed);
                }
            }
        }
    }

    void TriggerChangeManually(const std::string& filepath, FileAction action) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_callbacks.find(filepath);
        if (it != m_callbacks.end()) {
            it->second(filepath, action);
        }
    }

private:
    std::unordered_map<std::string, FileChangeCallback> m_callbacks;
    std::unordered_map<std::string, std::filesystem::file_time_type> m_lastWriteTimes;
    std::mutex m_mutex;
};

} // namespace starlight::assets
