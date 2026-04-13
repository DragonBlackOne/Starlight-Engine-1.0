#pragma once
#include "wicked/core/wiJobSystem.h"

namespace Vamos {
    using JobContext = wi::jobsystem::context;

    class JobSystem {
    public:
        static void Init() {
            wi::jobsystem::Initialize();
        }

        static void Execute(const wi::jobsystem::job_function_type& job) {
            static JobContext defaultCtx;
            wi::jobsystem::Execute(defaultCtx, job);
        }

        static void Execute(JobContext& ctx, const wi::jobsystem::job_function_type& job) {
            wi::jobsystem::Execute(ctx, job);
        }

        static void Dispatch(JobContext& ctx, uint32_t jobCount, uint32_t groupSize, const wi::jobsystem::job_function_type& job) {
            wi::jobsystem::Dispatch(ctx, jobCount, groupSize, job);
        }

        static void Wait(const JobContext& ctx) {
            wi::jobsystem::Wait(ctx);
        }

        static void Shutdown() {
            wi::jobsystem::ShutDown();
        }
    };
}
