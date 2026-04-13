#pragma once
#include <string>
#include <iostream>
#include <format>
#include <chrono>
#include <fstream>

namespace titan {
    enum class LogLevel {
        Info,
        Warn,
        Error,
        Fatal
    };

    class Log {
    public:
        static void Init(const std::string& filename = "titan_engine.log") {
            Get().m_file.open(filename);
        }

        template<typename... Args>
        static void Info(std::string_view fmt, Args&&... args) {
            LogMessage(LogLevel::Info, std::vformat(fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        static void Warn(std::string_view fmt, Args&&... args) {
            LogMessage(LogLevel::Warn, std::vformat(fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        static void Error(std::string_view fmt, Args&&... args) {
            LogMessage(LogLevel::Error, std::vformat(fmt, std::make_format_args(args...)));
        }

    private:
        static Log& Get() {
            static Log instance;
            return instance;
        }

        static void LogMessage(LogLevel level, const std::string& message) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto time_str = std::ctime(&time_t);
            if (time_str[std::strlen(time_str) - 1] == '\n') time_str[std::strlen(time_str) - 1] = '\0';

            const char* color = "\033[0m";
            const char* level_str = "INFO";

            switch (level) {
                case LogLevel::Info:  color = "\033[36m"; level_str = "INFO"; break;
                case LogLevel::Warn:  color = "\033[33m"; level_str = "WARN"; break;
                case LogLevel::Error: color = "\033[31m"; level_str = "ERROR"; break;
                case LogLevel::Fatal: color = "\033[41m"; level_str = "FATAL"; break;
            }

            std::string output = std::format("{} [{}] {} {}\033[0m", color, level_str, time_str, message);
            std::cout << output << std::endl;

            if (Get().m_file.is_open()) {
                Get().m_file << std::format("[{}] {} {}\n", level_str, time_str, message);
                Get().m_file.flush();
            }
        }

        std::ofstream m_file;
    };
}
