#pragma once
#include <string>
#include <iostream>
#include <format>
#include <chrono>
#include <fstream>
#include <vector>
#include <string_view>

namespace starlight {
    enum class LogLevel {
        Info,
        Warn,
        Error,
        Fatal,
        Debug
    };

    class Log {
    public:
        static void Init(const std::string& filename = "titan_engine.log");
        static const std::vector<std::string>& GetHistory();

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

        template<typename... Args>
        static void Debug(std::string_view fmt, Args&&... args) {
#ifdef _DEBUG
            LogMessage(LogLevel::Debug, std::vformat(fmt, std::make_format_args(args...)));
#endif
        }

    private:
        static Log& Get();
        static void LogMessage(LogLevel level, const std::string& message);

        std::ofstream m_file;
        std::vector<std::string> m_logHistory;
    };
}
