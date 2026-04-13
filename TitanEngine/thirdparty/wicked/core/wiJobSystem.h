#pragma once
#include <functional>
#include <cstdint>
#include <atomic>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace wi::jobsystem {
    struct context {
        std::atomic<uint32_t> counter{0};
    };

    struct JobArgs {
        uint32_t jobIndex;
        uint32_t groupIndex;
        uint32_t groupID;
    };

    void Initialize(uint32_t threadCount = 0);
    void ShutDown();

    void Execute(context& ctx, std::function<void(JobArgs)> job);
    void Dispatch(context& ctx, uint32_t jobCount, uint32_t groupSize, std::function<void(JobArgs)> job);
    void Wait(const context& ctx);
}
