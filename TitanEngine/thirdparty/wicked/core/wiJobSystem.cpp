#include "wiJobSystem.h"
#include <deque>
#include <iostream>

namespace wi::jobsystem {

    struct Job {
        std::function<void(JobArgs)> task;
        context* ctx;
        JobArgs args;
    };

    static struct InternalState {
        std::vector<std::thread> workers;
        std::deque<Job> jobQueue;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool shouldTerminate = false;
    } s_jobSystem;

    void Initialize(uint32_t threadCount) {
        if (threadCount == 0) {
            threadCount = std::thread::hardware_concurrency();
            if (threadCount > 1) threadCount--; // Keep one for main thread
        }

        s_jobSystem.shouldTerminate = false;
        for (uint32_t i = 0; i < threadCount; ++i) {
            s_jobSystem.workers.emplace_back([] {
                while (true) {
                    Job job;
                    {
                        std::unique_lock<std::mutex> lock(s_jobSystem.queueMutex);
                        s_jobSystem.condition.wait(lock, [] {
                            return s_jobSystem.shouldTerminate || !s_jobSystem.jobQueue.empty();
                        });

                        if (s_jobSystem.shouldTerminate && s_jobSystem.jobQueue.empty()) {
                            return;
                        }

                        job = std::move(s_jobSystem.jobQueue.front());
                        s_jobSystem.jobQueue.pop_front();
                    }

                    // Execute Job
                    job.task(job.args);
                    
                    if (job.ctx) {
                        job.ctx->counter.fetch_sub(1);
                    }
                }
            });
        }
    }

    void ShutDown() {
        {
            std::unique_lock<std::mutex> lock(s_jobSystem.queueMutex);
            s_jobSystem.shouldTerminate = true;
        }
        s_jobSystem.condition.notify_all();

        for (auto& worker : s_jobSystem.workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        s_jobSystem.workers.clear();
    }

    void Execute(context& ctx, std::function<void(JobArgs)> job) {
        ctx.counter.fetch_add(1);
        {
            std::unique_lock<std::mutex> lock(s_jobSystem.queueMutex);
            s_jobSystem.jobQueue.push_back({job, &ctx, {0, 0, 0}});
        }
        s_jobSystem.condition.notify_one();
    }

    void Dispatch(context& ctx, uint32_t jobCount, uint32_t groupSize, std::function<void(JobArgs)> job) {
        if (jobCount == 0 || groupSize == 0) return;

        ctx.counter.fetch_add(jobCount);
        {
            std::unique_lock<std::mutex> lock(s_jobSystem.queueMutex);
            for (uint32_t i = 0; i < jobCount; ++i) {
                JobArgs args;
                args.jobIndex = i;
                args.groupIndex = i / groupSize;
                args.groupID = i / groupSize;
                s_jobSystem.jobQueue.push_back({job, &ctx, args});
            }
        }
        s_jobSystem.condition.notify_all();
    }

    void Wait(const context& ctx) {
        while (ctx.counter.load() > 0) {
            std::this_thread::yield();
        }
    }
}
