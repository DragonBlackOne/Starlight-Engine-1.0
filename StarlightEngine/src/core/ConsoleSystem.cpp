// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "ConsoleSystem.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace starlight {

    ConsoleSystem& ConsoleSystem::Get() {
        static ConsoleSystem instance;
        return instance;
    }

    void ConsoleSystem::Log(const std::string& msg) {
        m_logs.push_back(msg);
        if (m_logs.size() > 100) m_logs.erase(m_logs.begin());
        std::cout << "[CONSOLE] " << msg << std::endl;
    }

    void ConsoleSystem::RegisterCVar(const std::string& name, float* ptr) {
        m_cvars[name] = ptr;
    }

    void ConsoleSystem::RegisterCVar(const std::string& name, int* ptr) {
        m_cvars[name] = ptr;
    }

    void ConsoleSystem::Execute(const std::string& command) {
        Log("] " + command);
        
        std::stringstream ss(command);
        std::string cmd;
        ss >> cmd;
        
        if (m_cvars.count(cmd)) {
            auto& val = m_cvars[cmd];
            std::string arg;
            if (ss >> arg) {
                if (std::holds_alternative<float*>(val)) {
                    *std::get<float*>(val) = std::stof(arg);
                    Log("  " + cmd + " set to " + arg);
                } else {
                    *std::get<int*>(val) = std::stoi(arg);
                    Log("  " + cmd + " set to " + arg);
                }
            } else {
                if (std::holds_alternative<float*>(val)) {
                    Log("  " + cmd + " = " + std::to_string(*std::get<float*>(val)));
                } else {
                    Log("  " + cmd + " = " + std::to_string(*std::get<int*>(val)));
                }
            }
        } else if (cmd == "help") {
            Log("Available CVars:");
            for (auto const& [name, _] : m_cvars) Log("  - " + name);
        } else {
            Log("Unknown command: " + cmd);
        }
    }

}
