// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Task Parallelism (Taskflow)
// Licença: MIT | Origem: taskflow/taskflow
// ============================================================

#include <taskflow/taskflow.hpp>
#include "Log.hpp"

namespace Vamos {
    // Taskflow é header-only e usa-se diretamente
    // Wrapper para conveniência
    class TaskSystem {
    public:
        TaskSystem(unsigned numThreads = std::thread::hardware_concurrency()) 
            : executor(numThreads) {
            VAMOS_CORE_INFO("[TaskSystem] Taskflow inicializado ({0} threads)", numThreads);
        }

        // Executar tarefas paralelas simples
        template<typename F>
        auto RunAsync(F&& func) {
            tf::Taskflow flow;
            flow.emplace(std::forward<F>(func));
            return executor.run(std::move(flow));
        }

        // Parallel for
        template<typename Iterator, typename F>
        void ParallelFor(Iterator begin, Iterator end, F&& func) {
            tf::Taskflow flow;
            flow.for_each(begin, end, std::forward<F>(func));
            executor.run(flow).wait();
        }

        // Pipeline de tarefas
        tf::Taskflow& CreateTaskflow() {
            taskflows.emplace_back();
            return taskflows.back();
        }

        void RunAndWait(tf::Taskflow& flow) {
            executor.run(flow).wait();
        }

        tf::Executor& GetExecutor() { return executor; }

    private:
        tf::Executor executor;
        std::vector<tf::Taskflow> taskflows;
    };
}
