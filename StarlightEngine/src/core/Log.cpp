#include "Log.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstring>

namespace starlight {

    void Log::Init(const std::string& filename) {
        Get().m_file.open(filename);
    }

    const std::vector<std::string>& Log::GetHistory() {
        return Get().m_logHistory;
    }

    Log& Log::Get() {
        static Log instance;
        return instance;
    }

    void Log::LogMessage(LogLevel level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        struct tm time_info;
        localtime_s(&time_info, &time_t);
        char time_buf[64];
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &time_info);

        const char* color = "\033[0m";
        const char* level_str = "INFO";

        switch (level) {
            case LogLevel::Info:  color = "\033[36m"; level_str = "INFO"; break;
            case LogLevel::Warn:  color = "\033[33m"; level_str = "WARN"; break;
            case LogLevel::Error: color = "\033[31m"; level_str = "ERROR"; break;
            case LogLevel::Fatal: color = "\033[41m"; level_str = "FATAL"; break;
            case LogLevel::Debug: color = "\033[32m"; level_str = "DEBUG"; break;
        }

        std::string output = std::format("{} [{}] {} {}\033[0m", color, level_str, time_buf, message);
        std::cout << output << std::endl;

        if (Get().m_file.is_open()) {
            Get().m_file << std::format("[{}] {} {}\n", level_str, time_buf, message);
            Get().m_file.flush();
        }

        Get().m_logHistory.push_back(std::format("[{}] {} {}", level_str, time_buf, message));
        if (Get().m_logHistory.size() > 1000) {
            Get().m_logHistory.erase(Get().m_logHistory.begin());
        }
    }

}
