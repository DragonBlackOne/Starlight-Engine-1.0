#include "Profiler.hpp"
#include <algorithm>
#include <numeric>

namespace starlight {

Profiler::Profiler() {
    m_frameHistory.assign(HISTORY_CAPACITY, 16.66f);
}

Profiler& Profiler::Get() {
    static Profiler instance;
    return instance;
}

void Profiler::BeginFrame() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_samples.clear();
}

void Profiler::EndFrame(float frameTimeMs) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_frameHistory[m_historyHead] = frameTimeMs;
    m_historyHead = (m_historyHead + 1) % HISTORY_CAPACITY;

    float sum = 0.0f;
    m_minFrameTime = 999999.0f;
    m_maxFrameTime = 0.0f;

    for (float t : m_frameHistory) {
        sum += t;
        if (t < m_minFrameTime) m_minFrameTime = t;
        if (t > m_maxFrameTime) m_maxFrameTime = t;
    }

    m_avgFrameTime = sum / static_cast<float>(HISTORY_CAPACITY);
    if (m_avgFrameTime > 0.0001f) {
        m_avgFPS = 1000.0f / m_avgFrameTime;
    }
}

void Profiler::BeginSample(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& sample = m_samples[name];
    sample.startTime = std::chrono::high_resolution_clock::now();
}

void Profiler::EndSample(const std::string& name) {
    auto now = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_samples.find(name);
    if (it != m_samples.end()) {
        float elapsedMs = std::chrono::duration<float, std::milli>(now - it->second.startTime).count();
        it->second.accumulatedMs += elapsedMs;
        it->second.count++;
    }
}

std::vector<ProfileSample> Profiler::GetActiveSamples() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ProfileSample> results;
    results.reserve(m_samples.size());
    for (const auto& [name, data] : m_samples) {
        results.push_back({name, data.accumulatedMs, data.count});
    }
    return results;
}

void Profiler::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_samples.clear();
    m_frameHistory.assign(HISTORY_CAPACITY, 16.66f);
    m_historyHead = 0;
    m_avgFrameTime = 16.66f;
    m_minFrameTime = 16.66f;
    m_maxFrameTime = 16.66f;
    m_avgFPS = 60.0f;
}

} // namespace starlight
