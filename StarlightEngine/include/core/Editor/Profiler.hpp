#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <array>
#include <mutex>
#include <algorithm>
#include <numeric>

namespace starlight::editor {

struct ProfileResult {
    std::string name;
    float elapsedMs = 0.0f;
    uint32_t threadID = 0;
};

class Profiler {
public:
    static constexpr size_t HISTORY_SIZE = 120;

    static Profiler& Get() {
        static Profiler instance;
        return instance;
    }

    void RecordTime(const std::string& name, float elapsedMs) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentFrameMetrics[name] = elapsedMs;

        auto& hist = m_history[name];
        if (hist.size() < HISTORY_SIZE) {
            hist.push_back(elapsedMs);
        } else {
            hist[m_historyIndices[name]] = elapsedMs;
            m_historyIndices[name] = (m_historyIndices[name] + 1) % HISTORY_SIZE;
        }
    }

    float GetMetric(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_currentFrameMetrics.find(name);
        return it != m_currentFrameMetrics.end() ? it->second : 0.0f;
    }

    float GetAverageMetric(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_history.find(name);
        if (it != m_history.end() && !it->second.empty()) {
            float sum = std::accumulate(it->second.begin(), it->second.end(), 0.0f);
            return sum / static_cast<float>(it->second.size());
        }
        return 0.0f;
    }

    float GetMaxMetric(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_history.find(name);
        if (it != m_history.end() && !it->second.empty()) {
            return *std::max_element(it->second.begin(), it->second.end());
        }
        return 0.0f;
    }

    const std::vector<float>& GetHistory(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        static const std::vector<float> empty;
        auto it = m_history.find(name);
        return it != m_history.end() ? it->second : empty;
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentFrameMetrics.clear();
        m_history.clear();
        m_historyIndices.clear();
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, float> m_currentFrameMetrics;
    std::unordered_map<std::string, std::vector<float>> m_history;
    std::unordered_map<std::string, size_t> m_historyIndices;
};

class ScopedTimer {
public:
    explicit ScopedTimer(std::string name)
        : m_name(std::move(name)),
          m_start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> duration = end - m_start;
        Profiler::Get().RecordTime(m_name, duration.count());
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

#define PROFILE_SCOPE(name) ::starlight::editor::ScopedTimer timer##__LINE__(name)

} // namespace starlight::editor
