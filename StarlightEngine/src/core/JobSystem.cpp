#include "JobSystem.hpp"
#include "Log.hpp"
#include <algorithm>

namespace starlight {

JobSystem* JobSystem::s_instance = nullptr;
std::unique_ptr<JobSystem> JobSystem::s_standaloneInstance = nullptr;

JobSystem::JobSystem() {
    if (!s_instance) {
        s_instance = this;
    }
}

JobSystem::~JobSystem() {
    OnShutdown();
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool JobSystem::OnInitialize([[maybe_unused]] const EngineContext& context) {
    (void)context;
    s_instance = this;
    if (m_running.load(std::memory_order_relaxed))
        return true;

    m_running.store(true, std::memory_order_release);
    uint32_t numThreads = std::max(2u, std::thread::hardware_concurrency());
    if (numThreads > 2) numThreads -= 1;

    m_workers.reserve(numThreads);
    for (uint32_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back(&JobSystem::WorkerLoop, this);
    }

    Log::Info("JobSystem: Initialized with {} worker threads.", numThreads);
    return true;
}

void JobSystem::OnUpdate([[maybe_unused]] float dt) {
    (void)dt;
}

void JobSystem::OnShutdown() {
    if (!m_running.load(std::memory_order_relaxed))
        return;

    m_running.store(false, std::memory_order_release);
    m_queueCV.notify_all();

    for (auto& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    m_workers.clear();

    std::lock_guard<std::mutex> lock(m_queueMutex);
    while (!m_jobQueue.empty()) {
        m_jobQueue.pop();
    }
    m_pendingJobs.store(0, std::memory_order_relaxed);
    m_activeJobs.store(0, std::memory_order_relaxed);
}

JobHandle JobSystem::Dispatch(std::function<void()> task, JobPriority priority) {
    if (!task)
        return JobHandle{};

    JobHandle handle;
    handle.id = m_nextJobId.fetch_add(1, std::memory_order_relaxed);
    handle.completed = std::make_shared<std::atomic<bool>>(false);

    InternalJob job;
    job.id = handle.id;
    job.priority = priority;
    job.task = std::move(task);
    job.completed = handle.completed;

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_jobQueue.push(std::move(job));
        m_pendingJobs.fetch_add(1, std::memory_order_relaxed);
    }

    m_queueCV.notify_one();
    return handle;
}

void JobSystem::ParallelFor(uint32_t count, std::function<void(uint32_t index)> taskFunc, uint32_t chunkSize) {
    if (count == 0 || !taskFunc)
        return;

    if (chunkSize == 0) chunkSize = 1;
    uint32_t numChunks = (count + chunkSize - 1) / chunkSize;
    std::vector<JobHandle> handles;
    handles.reserve(numChunks);

    for (uint32_t chunk = 0; chunk < numChunks; ++chunk) {
        uint32_t startIdx = chunk * chunkSize;
        uint32_t endIdx = std::min(startIdx + chunkSize, count);

        handles.push_back(Dispatch([startIdx, endIdx, &taskFunc]() {
            for (uint32_t i = startIdx; i < endIdx; ++i) {
                taskFunc(i);
            }
        }, JobPriority::High));
    }

    for (const auto& h : handles) {
        Wait(h);
    }
}

void JobSystem::Wait(const JobHandle& handle) {
    if (!handle.completed)
        return;

    while (!handle.IsDone()) {
        InternalJob jobToExecute;
        bool foundJob = false;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (!m_jobQueue.empty()) {
                jobToExecute = std::move(const_cast<InternalJob&>(m_jobQueue.top()));
                m_jobQueue.pop();
                m_pendingJobs.fetch_sub(1, std::memory_order_relaxed);
                m_activeJobs.fetch_add(1, std::memory_order_relaxed);
                foundJob = true;
            }
        }

        if (foundJob) {
            if (jobToExecute.task) {
                jobToExecute.task();
            }
            if (jobToExecute.completed) {
                jobToExecute.completed->store(true, std::memory_order_release);
            }
            m_activeJobs.fetch_sub(1, std::memory_order_relaxed);
            m_waitCV.notify_all();
        } else {
            std::this_thread::yield();
        }
    }
}

void JobSystem::WaitAll() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_waitCV.wait(lock, [this]() {
        return m_jobQueue.empty() && m_activeJobs.load(std::memory_order_relaxed) == 0;
    });
}

uint32_t JobSystem::GetQueuedJobCount() const {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return static_cast<uint32_t>(m_jobQueue.size());
}

void JobSystem::WorkerLoop() {
    while (m_running.load(std::memory_order_acquire)) {
        InternalJob job;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCV.wait(lock, [this]() {
                return !m_running.load(std::memory_order_acquire) || !m_jobQueue.empty();
            });

            if (!m_running.load(std::memory_order_acquire) && m_jobQueue.empty()) {
                return;
            }

            if (!m_jobQueue.empty()) {
                job = std::move(const_cast<InternalJob&>(m_jobQueue.top()));
                m_jobQueue.pop();
                m_pendingJobs.fetch_sub(1, std::memory_order_relaxed);
                m_activeJobs.fetch_add(1, std::memory_order_relaxed);
            } else {
                continue;
            }
        }

        if (job.task) {
            job.task();
        }

        if (job.completed) {
            job.completed->store(true, std::memory_order_release);
        }

        m_activeJobs.fetch_sub(1, std::memory_order_relaxed);
        m_waitCV.notify_all();
    }
}

// --- Static Helpers ---
void JobSystem::Initialize() {
    if (!s_instance && !s_standaloneInstance) {
        s_standaloneInstance = std::make_unique<JobSystem>();
        EngineContext ctx;
        s_standaloneInstance->OnInitialize(ctx);
        s_instance = s_standaloneInstance.get();
    }
}

void JobSystem::Shutdown() {
    if (s_standaloneInstance) {
        s_standaloneInstance->OnShutdown();
        s_standaloneInstance.reset();
        s_instance = nullptr;
    }
}

void JobSystem::Execute(const std::function<void(uint32_t)>& task) {
    if (s_instance) {
        s_instance->Dispatch([task]() { task(0); }, JobPriority::Normal);
    } else {
        task(0);
    }
}

void JobSystem::Execute(JobContext& ctx, const std::function<void(uint32_t)>& task) {
    if (s_instance) {
        ctx.handles.push_back(s_instance->Dispatch([task]() { task(0); }, JobPriority::Normal));
    } else {
        task(0);
    }
}

void JobSystem::Dispatch(JobContext& ctx, uint32_t count, uint32_t chunkSize, const std::function<void(uint32_t)>& task) {
    if (count == 0 || !task) return;
    if (chunkSize == 0) chunkSize = 1;
    uint32_t numChunks = (count + chunkSize - 1) / chunkSize;

    if (s_instance) {
        for (uint32_t chunk = 0; chunk < numChunks; ++chunk) {
            uint32_t startIdx = chunk * chunkSize;
            uint32_t endIdx = std::min(startIdx + chunkSize, count);

            ctx.handles.push_back(s_instance->Dispatch([startIdx, endIdx, &task]() {
                for (uint32_t i = startIdx; i < endIdx; ++i) {
                    task(i);
                }
            }, JobPriority::High));
        }
    } else {
        for (uint32_t i = 0; i < count; ++i) {
            task(i);
        }
    }
}

void JobSystem::Wait(const JobContext& ctx) {
    if (s_instance) {
        for (const auto& h : ctx.handles) {
            s_instance->Wait(h);
        }
    }
}

} // namespace starlight
