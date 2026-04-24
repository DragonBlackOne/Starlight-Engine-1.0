// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <cstdint>
#include <functional>

// Forward declaration para evitar inclusÃµes pesadas nos headers
namespace wi::jobsystem {
    struct context;
}

namespace titan {
    using JobContext = wi::jobsystem::context;

    class JobSystem {
    public:
        static void Initialize();
        static void Execute(const std::function<void(uint32_t)>& job);
        static void Execute(JobContext& ctx, const std::function<void(uint32_t)>& job);
        static void Dispatch(JobContext& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(uint32_t)>& job);
        static void Wait(const JobContext& ctx);
        static void Shutdown();
    };
}
