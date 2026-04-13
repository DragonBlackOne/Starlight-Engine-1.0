#include "wiJobSystem.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <iostream>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

namespace wi::jobsystem
{
    struct Job
    {
        job_function_type task;
        context* ctx;
        uint32_t groupID;
        uint32_t groupJobOffset;
        uint32_t groupJobEnd;
        uint32_t sharedmemory_size;

        void execute()
        {
            JobArgs args;
            args.groupID = groupID;
            args.sharedmemory = nullptr;
            if (sharedmemory_size > 0) {
                args.sharedmemory = alloca(sharedmemory_size);
            }

            for (uint32_t j = groupJobOffset; j < groupJobEnd; ++j)
            {
                args.jobIndex = j;
                args.groupIndex = j - groupJobOffset;
                args.isFirstJobInGroup = (j == groupJobOffset);
                args.isLastJobInGroup = (j == groupJobEnd - 1);
                task(args);
            }
            ctx->counter.fetch_sub(1);
        }
    };

    struct InternalState
    {
        std::vector<std::thread> threads;
        std::queue<Job> jobQueue;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::condition_variable waitCondition;
        std::atomic<bool> alive{ false };
        uint32_t numThreads = 0;

        void work()
        {
            while (alive)
            {
                Job job;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { return !jobQueue.empty() || !alive; });
                    if (!alive && jobQueue.empty()) return;
                    job = std::move(jobQueue.front());
                    jobQueue.pop();
                }
                job.execute();
                waitCondition.notify_all();
            }
        }
    } static internal_state;

    void Initialize(uint32_t maxThreadCount)
    {
        if (internal_state.alive) return;
        internal_state.alive = true;
        internal_state.numThreads = std::min(std::thread::hardware_concurrency(), maxThreadCount);
        if (internal_state.numThreads == 0) internal_state.numThreads = 1;

        for (uint32_t i = 0; i < internal_state.numThreads; ++i)
        {
            internal_state.threads.emplace_back(&InternalState::work, &internal_state);
        }
    }

    void ShutDown()
    {
        internal_state.alive = false;
        internal_state.condition.notify_all();
        for (auto& thread : internal_state.threads)
        {
            if (thread.joinable()) thread.join();
        }
        internal_state.threads.clear();
    }

    bool IsShuttingDown() { return !internal_state.alive; }

    uint32_t GetThreadCount(Priority priority) { return internal_state.numThreads; }

    void Execute(context& ctx, const job_function_type& task)
    {
        ctx.counter.fetch_add(1);
        Job job;
        job.ctx = &ctx;
        job.task = task;
        job.groupID = 0;
        job.groupJobOffset = 0;
        job.groupJobEnd = 1;
        job.sharedmemory_size = 0;

        {
            std::lock_guard<std::mutex> lock(internal_state.queueMutex);
            internal_state.jobQueue.push(job);
        }
        internal_state.condition.notify_one();
    }

    void Dispatch(context& ctx, uint32_t jobCount, uint32_t groupSize, const job_function_type& task, size_t sharedmemory_size)
    {
        if (jobCount == 0 || groupSize == 0) return;
        uint32_t groupCount = DispatchGroupCount(jobCount, groupSize);
        ctx.counter.fetch_add(groupCount);

        {
            std::lock_guard<std::mutex> lock(internal_state.queueMutex);
            for (uint32_t groupID = 0; groupID < groupCount; ++groupID)
            {
                Job job;
                job.ctx = &ctx;
                job.task = task;
                job.sharedmemory_size = (uint32_t)sharedmemory_size;
                job.groupID = groupID;
                job.groupJobOffset = groupID * groupSize;
                job.groupJobEnd = std::min(job.groupJobOffset + groupSize, jobCount);
                internal_state.jobQueue.push(job);
            }
        }
        internal_state.condition.notify_all();
    }

    uint32_t DispatchGroupCount(uint32_t jobCount, uint32_t groupSize)
    {
        return (jobCount + groupSize - 1) / groupSize;
    }

    bool IsBusy(const context& ctx) { return ctx.counter.load() > 0; }

    void Wait(const context& ctx)
    {
        while (IsBusy(ctx))
        {
            std::unique_lock<std::mutex> lock(internal_state.queueMutex);
            internal_state.waitCondition.wait(lock, [&ctx] { return !IsBusy(ctx); });
        }
    }

    uint32_t GetRemainingJobCount(const context& ctx) { return ctx.counter.load(); }
}
