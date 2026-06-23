#include "Log.hpp"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

namespace starlight {

void Log::Init(const std::string& filename) {
    Get().m_file.open(filename);
}

std::vector<std::string> Log::GetHistory() {
    std::lock_guard<std::mutex> lock(Get().m_mutex);
    return std::vector<std::string>(Get().m_logHistory.begin(), Get().m_logHistory.end());
}

void Log::ClearHistory() {
    std::lock_guard<std::mutex> lock(Get().m_mutex);
    Get().m_logHistory.clear();
    Get().m_logHistory.push_back("[INFO] Console history cleared.");
}

Log& Log::Get() {
    static Log instance;
    return instance;
}

std::string Log::StripAnsi(const std::string& str) {
    std::string res;
    res.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\033' && i + 1 < str.size() && str[i + 1] == '[') {
            i += 2;
            while (i < str.size() && str[i] != 'm') {
                ++i;
            }
        } else {
            res.push_back(str[i]);
        }
    }
    return res;
}

void Log::LogMessage(LogLevel level, const std::string& message) {
    if (level < Get().m_minLogLevel)
        return;

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    struct tm time_info;
    localtime_s(&time_info, &time_t);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &time_info);
    std::string timestamp = std::format("{}.{:03d}", time_buf, ms.count());

    const char* color = "\033[0m";
    const char* level_str = "INFO";

    switch (level) {
        case LogLevel::Debug:
            color = "\033[32m";
            level_str = "DEBUG";
            break;
        case LogLevel::Info:
            color = "\033[36m";
            level_str = "INFO";
            break;
        case LogLevel::Warn:
            color = "\033[33m";
            level_str = "WARN";
            break;
        case LogLevel::Error:
            color = "\033[31m";
            level_str = "ERROR";
            break;
        case LogLevel::Fatal:
            color = "\033[41m";
            level_str = "FATAL";
            break;
    }

    std::string output = std::format("{} [{}] {} {}\033[0m", color, level_str, timestamp, message);

    std::lock_guard<std::mutex> lock(Get().m_mutex);

    if (level >= LogLevel::Error) {
        std::cerr << output << std::endl;
    } else {
        std::cout << output << std::endl;
    }

    std::string cleanMessage = StripAnsi(message);
    std::string cleanLogEntry = std::format("[{}] {} {}", level_str, timestamp, cleanMessage);

    if (Get().m_file.is_open()) {
        Get().m_file << cleanLogEntry << "\n";
        Get().m_file.flush();
    }

    Get().m_logHistory.push_back(cleanLogEntry);
    if (Get().m_logHistory.size() > 1000) {
        Get().m_logHistory.pop_front();
    }

    if (level == LogLevel::Fatal) {
        std::abort();
    }
}

}  // namespace starlight
