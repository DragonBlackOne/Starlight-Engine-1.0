#include "DebugConsole.hpp"
#include "CVarSystem.hpp"
#include "ScriptSystem.hpp"
#include "InputSystem.hpp"
#include "Log.hpp"
#include "Renderer2D.hpp"
#include "Engine.hpp"
#include "Window.hpp"
#include "imgui.h"
#include "PathResolver.hpp"
#include <cmath>
#include <algorithm>
#include <variant>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

namespace starlight {

// Helper to get system memory usage on Windows
static size_t GetProcessMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#endif
    return 0;
}

DebugConsole::DebugConsole() {
    m_inputBuf[0] = '\0';
}

DebugConsole::~DebugConsole() {
    OnShutdown();
}

bool DebugConsole::OnInitialize(const EngineContext& context) {
    m_scriptSystem = context.engine->GetSystem<ScriptSystem>();
    m_cvarSystem = context.engine->GetSystem<CVarSystem>();
    
    // Load command history
    std::ifstream file(PathResolver::Resolve("assets/console_history.txt"));
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                m_commandHistory.push_back(line);
            }
        }
        Log::Info("DebugConsole: Loaded {} history commands.", m_commandHistory.size());
    }

    Log::Info("DebugConsole: Initialized (Press F1 or ` to toggle).");
    return true;
}

void DebugConsole::OnShutdown() {
    m_scriptSystem = nullptr;
    m_cvarSystem = nullptr;
}

void DebugConsole::OnUpdate(float dt) {
    // Check toggle shortcut (F1 or Tilde)
    auto* input = Engine::Get().GetSystem<InputSystem>();
    if (input) {
        // We can check if Console action or F1 was pressed
        if (input->IsActionJustPressed("Console") || input->IsKeyPressed(pal::KeyCode::F1)) {
            Toggle();
        }
    }

    // Smooth dropdown animation logic
    float target = m_isOpen ? 1.0f : 0.0f;
    if (m_animationProgress != target) {
        float speed = 12.0f;
        m_animationProgress += (target - m_animationProgress) * dt * speed;
        if (std::abs(m_animationProgress - target) < 0.005f) {
            m_animationProgress = target;
        }
    }
}

void DebugConsole::Toggle() {
    m_isOpen = !m_isOpen;
    if (m_isOpen) {
        // Reset command history index when opening
        m_historyPos = -1;
    }
}

int DebugConsole::TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
    DebugConsole* console = (DebugConsole*)data->UserData;
    return console->TextEditCallback(data);
}

int DebugConsole::TextEditCallback(ImGuiInputTextCallbackData* data) {
    switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion: {
            std::string prefix(data->Buf, data->BufTextLen);
            FilterAutocomplete(prefix);
            if (!m_autocompleteCandidates.empty()) {
                std::string candidate = m_autocompleteCandidates[m_autocompleteIndex];
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, candidate.c_str());
                m_autocompleteIndex = (m_autocompleteIndex + 1) % m_autocompleteCandidates.size();
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
            if (m_showAutocompleteMenu && !m_autocompleteCandidates.empty()) {
                if (data->EventKey == ImGuiKey_UpArrow) {
                    if (m_autocompleteIndex > 0) {
                        m_autocompleteIndex--;
                    } else {
                        m_autocompleteIndex = (int)m_autocompleteCandidates.size() - 1;
                    }
                } else if (data->EventKey == ImGuiKey_DownArrow) {
                    m_autocompleteIndex = (m_autocompleteIndex + 1) % m_autocompleteCandidates.size();
                }

                std::string candidate = m_autocompleteCandidates[m_autocompleteIndex];
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, candidate.c_str());
            } else {
                int prevHistoryPos = m_historyPos;
                if (data->EventKey == ImGuiKey_UpArrow) {
                    if (m_historyPos == -1) {
                        m_historyPos = (int)m_commandHistory.size() - 1;
                    } else if (m_historyPos > 0) {
                        m_historyPos--;
                    }
                } else if (data->EventKey == ImGuiKey_DownArrow) {
                    if (m_historyPos != -1) {
                        m_historyPos++;
                        if (m_historyPos >= (int)m_commandHistory.size()) {
                            m_historyPos = -1;
                        }
                    }
                }

                if (m_historyPos != prevHistoryPos) {
                    std::string cmd = (m_historyPos >= 0) ? m_commandHistory[m_historyPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, cmd.c_str());
                }
            }
            break;
        }
    }
    return 0;
}

void DebugConsole::FilterAutocomplete(const std::string& prefix) {
    m_autocompleteCandidates.clear();
    m_autocompleteIndex = 0;

    std::vector<std::string> builtins = { "help", "clear", "lua" };
    for (const auto& cmd : builtins) {
        if (cmd.compare(0, prefix.size(), prefix) == 0) {
            m_autocompleteCandidates.push_back(cmd);
        }
    }

    if (m_cvarSystem) {
        for (const auto& cv : m_cvarSystem->GetCVars()) {
            if (cv.name.compare(0, prefix.size(), prefix) == 0) {
                m_autocompleteCandidates.push_back(cv.name);
            }
        }
    }
}

void DebugConsole::Execute(const std::string& command) {
    // Log what the user typed
    Log::Info("] {}", command);
    
    // Add to history and persist (max 50 lines in file)
    if (m_commandHistory.empty() || m_commandHistory.back() != command) {
        m_commandHistory.push_back(command);
        
        std::string fullPath = PathResolver::ExeDir() + "/assets/console_history.txt";
        std::ofstream file(fullPath);
        if (file.is_open()) {
            size_t startIdx = (m_commandHistory.size() > 50) ? (m_commandHistory.size() - 50) : 0;
            for (size_t i = startIdx; i < m_commandHistory.size(); ++i) {
                file << m_commandHistory[i] << "\n";
            }
        }
    }
    m_historyPos = -1;

    // Split command and arguments
    size_t spacePos = command.find(' ');
    std::string cmd = (spacePos == std::string::npos) ? command : command.substr(0, spacePos);
    std::string args = (spacePos == std::string::npos) ? "" : command.substr(spacePos + 1);

    if (cmd == "help") {
        Log::Info("Available commands:");
        Log::Info("  help                - Show this message");
        Log::Info("  clear               - Clear log history");
        Log::Info("  lua <code>          - Execute Lua command");
        Log::Info("  <cvar> <value>      - Set CVar value");
        Log::Info("  <cvar>              - Get CVar value");
        if (m_cvarSystem) {
            Log::Info("Registered CVars:");
            for (const auto& cv : m_cvarSystem->GetCVars()) {
                Log::Info("  - {}: {}", cv.name, cv.description);
            }
        }
    } else if (cmd == "clear") {
        Log::ClearHistory();
    } else if (cmd == "lua") {
        if (m_scriptSystem && !args.empty()) {
            auto result = m_scriptSystem->GetLua().safe_script(args, sol::script_pass_on_error);
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("[Lua Error] {}", err.what());
            } else {
                Log::Info("[Lua] Success.");
            }
        } else {
            Log::Warn("Usage: lua <code>");
        }
    } else if (m_cvarSystem && m_cvarSystem->Exists(cmd)) {
        if (!args.empty()) {
            if (m_cvarSystem->SetFromString(cmd, args)) {
                Log::Info("CVar '{}' set to '{}'", cmd, args);
            } else {
                Log::Error("Failed to set CVar '{}' from '{}'", cmd, args);
            }
        } else {
            // Get CVar value and type
            CVarType t = m_cvarSystem->GetType(cmd);
            if (t == CVarType::Int) Log::Info("  {} = {} (Int)", cmd, m_cvarSystem->GetInt(cmd));
            else if (t == CVarType::Float) Log::Info("  {} = {:.4f} (Float)", cmd, m_cvarSystem->GetFloat(cmd));
            else if (t == CVarType::Bool) Log::Info("  {} = {} (Bool)", cmd, m_cvarSystem->GetBool(cmd) ? "true" : "false");
            else if (t == CVarType::String) Log::Info("  {} = '{}' (String)", cmd, m_cvarSystem->GetString(cmd));
        }
    } else {
        Log::Error("Unknown command or CVar: '{}'", cmd);
    }
}

void DebugConsole::OnRender() {
    if (m_animationProgress <= 0.0f && !m_isOpen) {
        return;
    }

    auto& engine = Engine::Get();
    float windowWidth = (float)engine.GetWindow().GetWidth();
    float windowHeight = (float)engine.GetWindow().GetHeight();

    // 40% height of screen for the console
    float consoleHeight = windowHeight * 0.40f;
    float currentHeight = consoleHeight * m_animationProgress;
    float currentY = -consoleHeight * (1.0f - m_animationProgress);

    // Styling: Cyberpunk synthwave dark obsidian with magenta borders
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.02f, 0.08f, 0.92f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.6f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.09f, 0.04f, 0.16f, 0.70f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.15f, 0.06f, 0.25f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));

    ImGui::SetNextWindowPos(ImVec2(0.0f, currentY));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, currentHeight));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoScrollbar;

    if (ImGui::Begin("##DebugConsoleWindow", nullptr, flags)) {
        // Status Top Bar
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "STARLIGHT ENGINE v7.0");
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::Text("FPS: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "%.1f", engine.GetTime().fps);
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::Text("Draw Calls: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "%u", Renderer2D::GetStats().drawCalls);
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::Text("RAM: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "%.2f MB", GetProcessMemoryUsage() / (1024.0f * 1024.0f));
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::Text("Filter:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150.0f);
        ImGui::InputText("##LogFilter", m_logFilterBuf, sizeof(m_logFilterBuf));
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Press F1 or ` to Close");

        ImGui::Separator();

        // 2-Column Split: [Left: Scroll Logs + Input] [Right: Telemetry/System info & active CVars]
        ImGui::Columns(2, "ConsoleSplit", false);
        ImGui::SetColumnWidth(0, windowWidth * 0.75f);
        ImGui::SetColumnWidth(1, windowWidth * 0.25f);

        // Left Column: Scroll Region
        float inputHeight = ImGui::GetFrameHeightWithSpacing();
        float scrollRegionHeight = currentHeight - ImGui::GetCursorPosY() - inputHeight - 12.0f;
        
        if (scrollRegionHeight > 0.0f) {
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, scrollRegionHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

            auto logs = Log::GetHistory();
            std::string filterStr(m_logFilterBuf);
            std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), [](unsigned char c) {
                return (char)std::tolower(c);
            });

            for (const auto& line : logs) {
                if (!filterStr.empty()) {
                    std::string lineLower = line;
                    std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(), [](unsigned char c) {
                        return (char)std::tolower(c);
                    });
                    if (lineLower.find(filterStr) == std::string::npos) {
                        continue;
                    }
                }
                ImVec4 color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
                if (line.find("[INFO]") != std::string::npos) {
                    color = ImVec4(0.0f, 0.85f, 1.0f, 1.0f); // Cyan
                } else if (line.find("[WARN]") != std::string::npos) {
                    color = ImVec4(1.0f, 0.75f, 0.0f, 1.0f); // Orange-Yellow
                } else if (line.find("[ERROR]") != std::string::npos || line.find("[FATAL]") != std::string::npos) {
                    color = ImVec4(1.0f, 0.1f, 0.5f, 1.0f);  // Hot Magenta
                } else if (line.find("[DEBUG]") != std::string::npos) {
                    color = ImVec4(0.2f, 1.0f, 0.4f, 1.0f);  // Neon Green
                }

                if (line.find("] ") != std::string::npos) {
                    color = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);   // Input Orange
                }

                ImGui::TextColored(color, "%s", line.c_str());
            }

            // Keep scroll at bottom if we are at bottom
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
        }

        // Left Column: Input Box
        ImGui::PushItemWidth(-1);
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue |
                                        ImGuiInputTextFlags_CallbackCompletion |
                                        ImGuiInputTextFlags_CallbackHistory;

        bool reclaimFocus = false;
        if (ImGui::InputText("##ConsoleInput", m_inputBuf, sizeof(m_inputBuf), inputFlags, TextEditCallbackStub, (void*)this)) {
            std::string command(m_inputBuf);
            if (!command.empty()) {
                Execute(command);
            }
            m_inputBuf[0] = '\0';
            reclaimFocus = true;
        }

        // Auto-focus input field
        if (reclaimFocus || (m_isOpen && m_animationProgress >= 1.0f && ImGui::IsWindowFocused())) {
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::PopItemWidth();

        // Visual Autocomplete overlay
        ImVec2 inputPos = ImGui::GetItemRectMin();
        std::string currentInput(m_inputBuf);
        if (!currentInput.empty()) {
            FilterAutocomplete(currentInput);
            m_showAutocompleteMenu = !m_autocompleteCandidates.empty();
        } else {
            m_showAutocompleteMenu = false;
        }

        if (m_showAutocompleteMenu) {
            float lineHeight = ImGui::GetTextLineHeightWithSpacing();
            float popupHeight = (m_autocompleteCandidates.size() * lineHeight) + 12.0f;
            ImVec2 popupPos = ImVec2(inputPos.x, inputPos.y - popupHeight - 4.0f);
            ImGui::SetNextWindowPos(popupPos);
            ImGui::SetNextWindowBgAlpha(0.95f);
            
            ImGuiWindowFlags popupFlags = ImGuiWindowFlags_NoTitleBar |
                                          ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoMove |
                                          ImGuiWindowFlags_NoCollapse |
                                          ImGuiWindowFlags_NoFocusOnAppearing |
                                          ImGuiWindowFlags_NoNav |
                                          ImGuiWindowFlags_AlwaysAutoResize |
                                          ImGuiWindowFlags_NoSavedSettings;

            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.02f, 0.08f, 0.95f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.6f, 0.8f));
            
            if (ImGui::Begin("##ConsoleAutocompletePopup", nullptr, popupFlags)) {
                for (size_t i = 0; i < m_autocompleteCandidates.size(); ++i) {
                    bool isSelected = ((int)i == m_autocompleteIndex);
                    if (isSelected) {
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "> %s", m_autocompleteCandidates[i].c_str());
                    } else {
                        ImGui::Text("  %s", m_autocompleteCandidates[i].c_str());
                    }
                }
            }
            ImGui::End();
            ImGui::PopStyleColor(2);
        }

        // Right Column: Telemetry HUD & active CVars
        ImGui::NextColumn();
        
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "CPU PROFILER");
        ImGui::Separator();
        
        const auto& prof = engine.GetProfilerStats();
        ImGui::Text("Render:  "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "%.2f ms", prof.renderTime);
        ImGui::Text("Scripts: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "%.2f ms", prof.scriptTime);
        ImGui::Text("Physics: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "%.2f ms", prof.physicsTime);
        ImGui::Text("Audio:   "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "%.2f ms", prof.audioTime);
        ImGui::Text("Update:  "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.7f, 0.4f, 1.0f, 1.0f), "%.2f ms", prof.updateTime);
        
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "CVARS MANAGER");
        ImGui::Separator();
        
        float rightPanelHeight = currentHeight - ImGui::GetCursorPosY() - 12.0f;
        if (rightPanelHeight > 0.0f && m_cvarSystem) {
            ImGui::BeginChild("CVarScrollPanel", ImVec2(0, rightPanelHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
            for (const auto& cv : m_cvarSystem->GetCVars()) {
                std::string valStr = std::visit([](const auto& arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int>) {
                        return std::to_string(arg);
                    } else if constexpr (std::is_same_v<T, float>) {
                        return std::to_string(arg);
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return arg ? "true" : "false";
                    } else {
                        return arg;
                    }
                }, cv.value);

                ImGui::TextColored(ImVec4(0.0f, 0.85f, 1.0f, 1.0f), "%s", cv.name.c_str());
                ImGui::SameLine();
                ImGui::Text(" = %s", valStr.c_str());
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", cv.description.c_str());
                }
            }
            ImGui::EndChild();
        }

        ImGui::End();
    }
    ImGui::PopStyleColor(5);
}

} // namespace starlight
