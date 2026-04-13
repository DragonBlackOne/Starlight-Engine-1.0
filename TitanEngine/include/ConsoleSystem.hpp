#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>

namespace titan {

    using CVarValue = std::variant<float, int, std::string>;

    class ConsoleSystem {
    public:
        static ConsoleSystem& Get();

        void Log(const std::string& msg);
        void Execute(const std::string& command);

        void RegisterCVar(const std::string& name, float* ptr);
        void RegisterCVar(const std::string& name, int* ptr);

        const std::vector<std::string>& GetLogs() const { return m_logs; }
        bool IsVisible() const { return m_visible; }
        void Toggle() { m_visible = !m_visible; }

    private:
        ConsoleSystem() = default;
        std::vector<std::string> m_logs;
        std::map<std::string, std::variant<float*, int*>> m_cvars;
        bool m_visible = false;
    };

}
