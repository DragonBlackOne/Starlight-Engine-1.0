// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <functional>
#include <atomic>
#include <cstdint>

namespace wi::jobsystem
{
    struct JobArgs
    {
        uint32_t jobIndex;
        uint32_t groupID;
        uint32_t groupIndex;
        bool isFirstJobInGroup;
        bool isLastJobInGroup;
        void* sharedmemory;
    };

    using job_function_type = std::function<void(JobArgs)>;

    enum class Priority
    {
        High,
        Low,
        Streaming,
        Count
    };

    struct context
    {
        std::atomic<uint32_t> counter{ 0 };
        Priority priority = Priority::High;
    };

    void Initialize(uint32_t maxThreadCount = ~0u);
    void ShutDown();
    bool IsShuttingDown();
    uint32_t GetThreadCount(Priority priority = Priority::High);
    void Execute(context& ctx, const job_function_type& task);
    void Dispatch(context& ctx, uint32_t jobCount, uint32_t groupSize, const job_function_type& task, size_t sharedmemory_size = 0);
    uint32_t DispatchGroupCount(uint32_t jobCount, uint32_t groupSize);
    bool IsBusy(const context& ctx);
    void Wait(const context& ctx);
    uint32_t GetRemainingJobCount(const context& ctx);
}
