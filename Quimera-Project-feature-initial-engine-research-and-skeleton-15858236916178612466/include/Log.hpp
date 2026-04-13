#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace Vamos {
    class Log {
    public:
        static void Init() {
            spdlog::set_pattern("%^[%T] %n: %v%$");
            s_CoreLogger = spdlog::stdout_color_mt("VAMOS");
            s_CoreLogger->set_level(spdlog::level::trace);
        }

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
    };
}

// Macros de conveniência
#define VAMOS_CORE_TRACE(...)    ::Vamos::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VAMOS_CORE_INFO(...)     ::Vamos::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VAMOS_CORE_WARN(...)     ::Vamos::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VAMOS_CORE_ERROR(...)    ::Vamos::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VAMOS_CORE_CRITICAL(...) ::Vamos::Log::GetCoreLogger()->critical(__VA_ARGS__)
