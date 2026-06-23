#pragma once
#include <chrono>
#include <deque>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace starlight {
enum class LogLevel { Debug = 0, Info = 1, Warn = 2, Error = 3, Fatal = 4 };

struct LogTag {
    std::string_view name;
};

class Log {
public:
    static void Init(const std::string& filename = "starlight_engine.log");
    static std::vector<std::string> GetHistory();

    static void SetMinLogLevel(LogLevel level) {
        Get().m_minLogLevel = level;
    }

    static LogLevel GetMinLogLevel() {
        return Get().m_minLogLevel;
    }

    static void ClearHistory();

    // Info
    template<typename... Args>
    static void Info(std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Info, std::vformat(fmt, std::make_format_args(args...)));
    }
    template<typename... Args>
    static void Info(LogTag tag, std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Info, std::format("{} {}", tag.name, std::vformat(fmt, std::make_format_args(args...))));
    }

    // Warn
    template<typename... Args>
    static void Warn(std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Warn, std::vformat(fmt, std::make_format_args(args...)));
    }
    template<typename... Args>
    static void Warn(LogTag tag, std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Warn, std::format("{} {}", tag.name, std::vformat(fmt, std::make_format_args(args...))));
    }

    // Error
    template<typename... Args>
    static void Error(std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Error, std::vformat(fmt, std::make_format_args(args...)));
    }
    template<typename... Args>
    static void Error(LogTag tag, std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Error, std::format("{} {}", tag.name, std::vformat(fmt, std::make_format_args(args...))));
    }

    // Debug
    template<typename... Args>
    static void Debug(std::string_view fmt, Args&&... args) {
#ifdef _DEBUG
        LogMessage(LogLevel::Debug, std::vformat(fmt, std::make_format_args(args...)));
#else
        (void)fmt;
        ((void)args, ...);
#endif
    }
    template<typename... Args>
    static void Debug(LogTag tag, std::string_view fmt, Args&&... args) {
#ifdef _DEBUG
        LogMessage(LogLevel::Debug, std::format("{} {}", tag.name, std::vformat(fmt, std::make_format_args(args...))));
#else
        (void)tag;
        (void)fmt;
        ((void)args, ...);
#endif
    }

    // Fatal
    template<typename... Args>
    static void Fatal(std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Fatal, std::vformat(fmt, std::make_format_args(args...)));
    }
    template<typename... Args>
    static void Fatal(LogTag tag, std::string_view fmt, Args&&... args) {
        LogMessage(LogLevel::Fatal, std::format("{} {}", tag.name, std::vformat(fmt, std::make_format_args(args...))));
    }

private:
    static Log& Get();
    static void LogMessage(LogLevel level, const std::string& message);
    static std::string StripAnsi(const std::string& str);

    std::ofstream m_file;
    std::deque<std::string> m_logHistory;
    std::mutex m_mutex;
    LogLevel m_minLogLevel = LogLevel::Info;
};
}  // namespace starlight
