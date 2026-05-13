// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <cstdint>
#include <functional>
#include "EngineSystem.hpp"

// Forward declaration para evitar inclusÃƒÆ’Ã†â€™Ãƒâ€šÃ‚Âµes pesadas nos headers
namespace wi::jobsystem {
    struct context;
}

namespace starlight {
    using JobContext = wi::jobsystem::context;

    class JobSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; Initialize(); return true; }
        void OnShutdown() override { Shutdown(); }
        const char* GetName() const override { return "JobSystem"; }

        static void Initialize();
        static void Execute(const std::function<void(uint32_t)>& job);
        static void Execute(JobContext& ctx, const std::function<void(uint32_t)>& job);
        static void Dispatch(JobContext& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(uint32_t)>& job);
        static void Wait(const JobContext& ctx);
        static void Shutdown();
    };
}
