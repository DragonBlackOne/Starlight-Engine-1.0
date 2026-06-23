#pragma once
#include "EngineSystem.hpp"
#include <string>
#include <vector>
#include <mutex>
#include <sol/sol.hpp>

#include "imgui.h"

namespace starlight {

class ScriptSystem;
class CVarSystem;

class DebugConsole : public ISystem {
public:
    DebugConsole();
    ~DebugConsole();

    // ISystem interface
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    const char* GetName() const override { return "DebugConsole"; }
    bool IsMainThreadOnly() const override { return true; }
    int GetPriority() const override { return 100; } // Render high, near editor

    void Toggle();
    bool IsOpen() const { return m_isOpen; }
    void AddLogMessage(const std::string& msg, const std::string& category = "SYSTEM");

    // Execute console command
    void Execute(const std::string& command);

private:
    // ImGui Input Callback
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void FilterAutocomplete(const std::string& prefix);

    bool m_isOpen = false;
    float m_animationProgress = 0.0f; // For sliding animation (0 to 1)

    char m_inputBuf[512];
    char m_logFilterBuf[128] = "";
    std::vector<std::string> m_commandHistory;
    int m_historyPos = -1; // -1: new line, otherwise index into m_commandHistory

    std::vector<std::string> m_autocompleteCandidates;
    int m_autocompleteIndex = 0;
    bool m_showAutocompleteMenu = false;

    // Custom logs injected directly into console
    struct ConsoleLog {
        std::string text;
        std::string category; // SYSTEM, INPUT, WARN, ERROR, etc.
    };
    std::vector<ConsoleLog> m_customLogs;
    std::mutex m_logMutex;

    // References
    ScriptSystem* m_scriptSystem = nullptr;
    CVarSystem* m_cvarSystem = nullptr;
};

} // namespace starlight
