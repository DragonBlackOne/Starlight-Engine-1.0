#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace starlight {

struct ProfileSample {
    std::string name;
    float durationMs = 0.0f;
    uint32_t callCount = 0;
};

class Profiler {
public:
    static Profiler& Get();

    void BeginFrame();
    void EndFrame(float frameTimeMs);

    void BeginSample(const std::string& name);
    void EndSample(const std::string& name);

    // Frame-time historical data (120 historical frames)
    const std::vector<float>& GetFrameTimeHistory() const { return m_frameHistory; }
    float GetAvgFrameTime() const { return m_avgFrameTime; }
    float GetMinFrameTime() const { return m_minFrameTime; }
    float GetMaxFrameTime() const { return m_maxFrameTime; }
    float GetAvgFPS() const { return m_avgFPS; }

    std::vector<ProfileSample> GetActiveSamples() const;
    void Clear();

private:
    Profiler();
    ~Profiler() = default;

    static constexpr size_t HISTORY_CAPACITY = 120;
    std::vector<float> m_frameHistory;
    size_t m_historyHead = 0;

    float m_avgFrameTime = 16.66f;
    float m_minFrameTime = 16.66f;
    float m_maxFrameTime = 16.66f;
    float m_avgFPS = 60.0f;

    struct SampleData {
        std::chrono::high_resolution_clock::time_point startTime;
        float accumulatedMs = 0.0f;
        uint32_t count = 0;
    };

    mutable std::mutex m_mutex;
    std::unordered_map<std::string, SampleData> m_samples;
};

class ScopedProfileSample {
public:
    ScopedProfileSample(const std::string& name) : m_name(name) {
        Profiler::Get().BeginSample(m_name);
    }
    ~ScopedProfileSample() {
        Profiler::Get().EndSample(m_name);
    }
private:
    std::string m_name;
};

#define STARLIGHT_PROFILE_SCOPE(name) ::starlight::ScopedProfileSample _scoped_prof_##__LINE__(name)

} // namespace starlight
