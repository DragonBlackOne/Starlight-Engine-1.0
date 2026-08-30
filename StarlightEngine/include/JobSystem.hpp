#pragma once
#include "EngineSystem.hpp"
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <future>

namespace starlight {

enum class JobPriority : uint8_t {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

struct JobHandle {
    uint64_t id = 0;
    std::shared_ptr<std::atomic<bool>> completed;

    bool IsDone() const {
        return !completed || completed->load(std::memory_order_acquire);
    }
};

struct JobContext {
    std::vector<JobHandle> handles;
};

class JobSystem : public ISystem {
public:
    JobSystem();
    ~JobSystem() override;

    bool OnInitialize(const EngineContext& context) override;
    void OnUpdate(float dt) override;
    void OnShutdown() override;

    const char* GetName() const override { return "JobSystem"; }
    bool IsMainThreadOnly() const override { return false; }
    int GetPriority() const override { return 10; }

    // Instance APIs
    JobHandle Dispatch(std::function<void()> task, JobPriority priority = JobPriority::Normal);
    void ParallelFor(uint32_t count, std::function<void(uint32_t index)> taskFunc, uint32_t chunkSize = 32);
    void Wait(const JobHandle& handle);
    void WaitAll();

    uint32_t GetWorkerCount() const { return static_cast<uint32_t>(m_workers.size()); }
    uint32_t GetQueuedJobCount() const;
    uint32_t GetActiveJobCount() const { return m_activeJobs.load(std::memory_order_relaxed); }

    // Static Convenience & Compatibility Interface
    static void Initialize();
    static void Shutdown();
    static void Execute(const std::function<void(uint32_t)>& task);
    static void Execute(JobContext& ctx, const std::function<void(uint32_t)>& task);
    static void Dispatch(JobContext& ctx, uint32_t count, uint32_t chunkSize, const std::function<void(uint32_t)>& task);
    static void Wait(const JobContext& ctx);

private:
    struct InternalJob {
        uint64_t id = 0;
        JobPriority priority = JobPriority::Normal;
        std::function<void()> task;
        std::shared_ptr<std::atomic<bool>> completed;

        bool operator<(const InternalJob& other) const {
            return static_cast<uint8_t>(priority) < static_cast<uint8_t>(other.priority);
        }
    };

    void WorkerLoop();

    std::vector<std::thread> m_workers;
    std::priority_queue<InternalJob> m_jobQueue;
    mutable std::mutex m_queueMutex;
    std::condition_variable m_queueCV;
    std::condition_variable m_waitCV;

    std::atomic<bool> m_running{false};
    std::atomic<uint64_t> m_nextJobId{1};
    std::atomic<uint32_t> m_activeJobs{0};
    std::atomic<uint32_t> m_pendingJobs{0};

    static JobSystem* s_instance;
    static std::unique_ptr<JobSystem> s_standaloneInstance;
};

} // namespace starlight
