#include "JobSystem.hpp"
#include "Log.hpp"
#include "wicked/core/wiJobSystem.h"

namespace titan {
    void JobSystem::Initialize() {
        wi::jobsystem::Initialize();
        Log::Info("Job System (Wicked) inicializado.");
    }

    void JobSystem::Execute(const std::function<void(uint32_t)>& job) {
        static JobContext defaultCtx;
        wi::jobsystem::Execute(defaultCtx, [job](wi::jobsystem::JobArgs args) {
            job(args.jobIndex);
        });
    }

    void JobSystem::Execute(JobContext& ctx, const std::function<void(uint32_t)>& job) {
        wi::jobsystem::Execute(ctx, [job](wi::jobsystem::JobArgs args) {
            job(args.jobIndex);
        });
    }

    void JobSystem::Dispatch(JobContext& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(uint32_t)>& job) {
        wi::jobsystem::Dispatch(ctx, jobCount, groupSize, [job](wi::jobsystem::JobArgs args) {
            job(args.jobIndex);
        });
    }

    void JobSystem::Wait(const JobContext& ctx) {
        wi::jobsystem::Wait(ctx);
    }

    void JobSystem::Shutdown() {
        wi::jobsystem::ShutDown();
        Log::Info("Job System finalizado.");
    }
}
