#include "EditorSystem.hpp"
#include "Engine.hpp"
#include "PhysicsSystem.hpp"
#include "CVarSystem.hpp"
#include "Renderer.hpp"
#include "Log.hpp"
#include "CoreMinimal.hpp"
#include "Components.hpp"
#include <RmlUi/Core.h>
#include "RmlRenderInterfaceGL3.hpp"
#include "RmlSystemInterfaceSDL.hpp"
#include "SpriteAnimationComponent.hpp"
#include "SceneSerializer.hpp"
#include "FileWatcher.hpp"
#include "ScriptSystem.hpp"
#include "InputSystem.hpp"
#include "PrefabSystem.hpp"
#include <json.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>
#include <SDL2/SDL.h>
#include <RmlUi/Core/Input.h>
#include "PathResolver.hpp"
#include "Renderer2D.hpp"
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <deque>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;

namespace starlight {

struct ModernPin {
    int id;
    std::string name;
};

struct ModernNode {
    int id;
    std::string name;
    std::string type;
    ImColor color;
    std::vector<ModernPin> inputPins;
    std::vector<ModernPin> outputPins;
    ImVec2 pos;
    float value = 0.0f;
};

struct ModernLink {
    int id;
    int fromPin;
    int toPin;
};

namespace {

static int s_selectedShaderNode = -1;
static int s_selectedBtNode = -1;

static std::unique_ptr<starlight::RmlRenderInterfaceGL3> s_rmlRenderInterface;
static std::unique_ptr<starlight::RmlSystemInterfaceSDL> s_rmlSystemInterface;
static Rml::Context* s_rmlContext = nullptr;

struct ConsoleEntry {
    LogLevel level;
    std::string message;
};

std::deque<ConsoleEntry> s_consoleBuffer;
constexpr size_t MAX_CONSOLE_LINES = 500;

nlohmann::json s_sandboxSnapshot;

void ConsoleSink(LogLevel level, const std::string& msg) {
    s_consoleBuffer.push_back({level, msg});
    if (s_consoleBuffer.size() > MAX_CONSOLE_LINES) {
        s_consoleBuffer.pop_front();
    }
}

bool ContainsCaseInsensitive(const std::string& source, const std::string& search) {
    if (search.empty()) return true;
    auto it = std::search(
        source.begin(), source.end(),
        search.begin(), search.end(),
        [](unsigned char ch1, unsigned char ch2) {
            return std::tolower(ch1) == std::tolower(ch2);
        }
    );
    return it != source.end();
}

nlohmann::json CopyComponentToJson(entt::registry& registry, entt::entity entity, const std::string& componentType) {
    nlohmann::json json;
    json["ComponentType"] = componentType;
    if (componentType == "Transform") {
        auto& c = registry.get<TransformComponent>(entity);
        json["position"] = { c.position.x, c.position.y, c.position.z };
        json["rotation"] = { c.rotation.w, c.rotation.x, c.rotation.y, c.rotation.z };
        json["scale"] = { c.scale.x, c.scale.y, c.scale.z };
    } else if (componentType == "Sprite") {
        auto& c = registry.get<SpriteComponent>(entity);
        json["color"] = { c.color.r, c.color.g, c.color.b, c.color.a };
        json["pivot"] = { c.pivot.x, c.pivot.y };
        json["layer"] = c.layer;
        json["order"] = c.orderInLayer;
        json["flipX"] = c.flipX;
        json["flipY"] = c.flipY;
        json["visible"] = c.visible;
    } else if (componentType == "Camera2D") {
        auto& c = registry.get<Camera2DComponent>(entity);
        json["zoom"] = c.zoom;
        json["rotation"] = c.rotation;
        json["primary"] = c.primary;
    } else if (componentType == "SpriteAnimation") {
        auto& c = registry.get<SpriteAnimationComponent>(entity);
        json["fps"] = c.fps;
        json["playing"] = c.playing;
        json["looping"] = c.looping;
    } else if (componentType == "Tilemap") {
        auto& c = registry.get<TilemapComponent>(entity);
        json["tileSize"] = c.tileSize;
        json["tilesetColumns"] = c.tilesetColumns;
        json["tilesetRows"] = c.tilesetRows;
        json["color"] = { c.color.r, c.color.g, c.color.b, c.color.a };
        json["layer"] = c.layer;
    } else if (componentType == "Mesh") {
        auto& c = registry.get<MeshComponent>(entity);
        json["albedo"] = { c.material.albedo.r, c.material.albedo.g, c.material.albedo.b };
        json["metallic"] = c.material.metallic;
        json["roughness"] = c.material.roughness;
        json["isPBR"] = c.material.isPBR;
        json["visible"] = c.isVisible;
    } else if (componentType == "PointLight") {
        auto& c = registry.get<PointLightComponent>(entity);
        json["color"] = { c.color.r, c.color.g, c.color.b };
        json["intensity"] = c.intensity;
    } else if (componentType == "Retro") {
        auto& c = registry.get<RetroComponent>(entity);
        json["map_x"] = c.map_x;
        json["map_y"] = c.map_y;
        json["map_z"] = c.map_z;
        json["horizon"] = c.horizon;
        json["pitch"] = c.pitch;
        json["skyColor"] = { c.skyColor.r, c.skyColor.g, c.skyColor.b };
        json["active"] = c.active;
    } else if (componentType == "Camera3D") {
        auto& c = registry.get<CameraComponent>(entity);
        json["fov"] = c.fov;
        json["nearPlane"] = c.nearPlane;
        json["farPlane"] = c.farPlane;
        json["primary"] = c.primary;
    } else if (componentType == "ReverbZone") {
        auto& c = registry.get<ReverbZoneComponent>(entity);
        json["minDistance"] = c.minDistance;
        json["maxDistance"] = c.maxDistance;
        json["reverbFactor"] = c.reverbFactor;
        json["active"] = c.active;
    } else if (componentType == "CharacterController") {
        auto& c = registry.get<CharacterControllerComponent>(entity);
        json["height"] = c.height;
        json["radius"] = c.radius;
        json["maxSlopeAngle"] = c.maxSlopeAngle;
        json["jumpStrength"] = c.jumpStrength;
        json["speed"] = c.speed;
        json["velocity"] = { c.velocity.x, c.velocity.y, c.velocity.z };
    } else if (componentType == "FootIK") {
        auto& c = registry.get<FootIKComponent>(entity);
        json["leftFootOffset"] = { c.leftFootOffset.x, c.leftFootOffset.y, c.leftFootOffset.z };
        json["rightFootOffset"] = { c.rightFootOffset.x, c.rightFootOffset.y, c.rightFootOffset.z };
        json["enabled"] = c.enabled;
    } else if (componentType == "NavAgent") {
        auto& c = registry.get<NavAgentComponent>(entity);
        json["target"] = { c.target.x, c.target.y, c.target.z };
        json["velocity"] = { c.velocity.x, c.velocity.y, c.velocity.z };
        json["radius"] = c.radius;
        json["maxSpeed"] = c.maxSpeed;
        json["active"] = c.active;
    } else if (componentType == "SoftBody") {
        auto& c = registry.get<SoftBodyComponent>(entity);
        json["mass"] = c.mass;
        json["pressure"] = c.pressure;
        json["enabled"] = c.enabled;
    }
    return json;
}

void PasteComponentFromJson(entt::registry& registry, entt::entity entity, const nlohmann::json& json) {
    if (!json.contains("ComponentType")) return;
    std::string componentType = json["ComponentType"];
    if (componentType == "Transform") {
        auto& c = registry.get_or_emplace<TransformComponent>(entity);
        if (json.contains("position")) c.position = { json["position"][0], json["position"][1], json["position"][2] };
        if (json.contains("rotation")) c.rotation = { json["rotation"][0], json["rotation"][1], json["rotation"][2], json["rotation"][3] };
        if (json.contains("scale")) c.scale = { json["scale"][0], json["scale"][1], json["scale"][2] };
    } else if (componentType == "Sprite") {
        auto& c = registry.get_or_emplace<SpriteComponent>(entity);
        if (json.contains("color")) c.color = { json["color"][0], json["color"][1], json["color"][2], json["color"][3] };
        if (json.contains("pivot")) c.pivot = { json["pivot"][0], json["pivot"][1] };
        if (json.contains("layer")) c.layer = json["layer"];
        if (json.contains("order")) c.orderInLayer = json["order"];
        if (json.contains("flipX")) c.flipX = json["flipX"];
        if (json.contains("flipY")) c.flipY = json["flipY"];
        if (json.contains("visible")) c.visible = json["visible"];
    } else if (componentType == "Camera2D") {
        auto& c = registry.get_or_emplace<Camera2DComponent>(entity);
        if (json.contains("zoom")) c.zoom = json["zoom"];
        if (json.contains("rotation")) c.rotation = json["rotation"];
        if (json.contains("primary")) c.primary = json["primary"];
    } else if (componentType == "SpriteAnimation") {
        auto& c = registry.get_or_emplace<SpriteAnimationComponent>(entity);
        if (json.contains("fps")) c.fps = json["fps"];
        if (json.contains("playing")) c.playing = json["playing"];
        if (json.contains("looping")) c.looping = json["looping"];
    } else if (componentType == "Tilemap") {
        auto& c = registry.get_or_emplace<TilemapComponent>(entity);
        if (json.contains("tileSize")) c.tileSize = json["tileSize"];
        if (json.contains("tilesetColumns")) c.tilesetColumns = json["tilesetColumns"];
        if (json.contains("tilesetRows")) c.tilesetRows = json["tilesetRows"];
        if (json.contains("color")) c.color = { json["color"][0], json["color"][1], json["color"][2], json["color"][3] };
        if (json.contains("layer")) c.layer = json["layer"];
    } else if (componentType == "Mesh") {
        auto& c = registry.get_or_emplace<MeshComponent>(entity);
        if (json.contains("albedo")) c.material.albedo = { json["albedo"][0], json["albedo"][1], json["albedo"][2] };
        if (json.contains("metallic")) c.material.metallic = json["metallic"];
        if (json.contains("roughness")) c.material.roughness = json["roughness"];
        if (json.contains("isPBR")) c.material.isPBR = json["isPBR"];
        if (json.contains("visible")) c.isVisible = json["visible"];
    } else if (componentType == "PointLight") {
        auto& c = registry.get_or_emplace<PointLightComponent>(entity);
        if (json.contains("color")) c.color = { json["color"][0], json["color"][1], json["color"][2] };
        if (json.contains("intensity")) c.intensity = json["intensity"];
    } else if (componentType == "Retro") {
        auto& c = registry.get_or_emplace<RetroComponent>(entity);
        if (json.contains("map_x")) c.map_x = json["map_x"];
        if (json.contains("map_y")) c.map_y = json["map_y"];
        if (json.contains("map_z")) c.map_z = json["map_z"];
        if (json.contains("horizon")) c.horizon = json["horizon"];
        if (json.contains("pitch")) c.pitch = json["pitch"];
        if (json.contains("skyColor")) c.skyColor = { json["skyColor"][0], json["skyColor"][1], json["skyColor"][2] };
        if (json.contains("active")) c.active = json["active"];
    } else if (componentType == "Camera3D") {
        auto& c = registry.get_or_emplace<CameraComponent>(entity);
        if (json.contains("fov")) c.fov = json["fov"];
        if (json.contains("nearPlane")) c.nearPlane = json["nearPlane"];
        if (json.contains("farPlane")) c.farPlane = json["farPlane"];
        if (json.contains("primary")) c.primary = json["primary"];
    } else if (componentType == "ReverbZone") {
        auto& c = registry.get_or_emplace<ReverbZoneComponent>(entity);
        if (json.contains("minDistance")) c.minDistance = json["minDistance"];
        if (json.contains("maxDistance")) c.maxDistance = json["maxDistance"];
        if (json.contains("reverbFactor")) c.reverbFactor = json["reverbFactor"];
        if (json.contains("active")) c.active = json["active"];
    } else if (componentType == "CharacterController") {
        auto& c = registry.get_or_emplace<CharacterControllerComponent>(entity);
        if (json.contains("height")) c.height = json["height"];
        if (json.contains("radius")) c.radius = json["radius"];
        if (json.contains("maxSlopeAngle")) c.maxSlopeAngle = json["maxSlopeAngle"];
        if (json.contains("jumpStrength")) c.jumpStrength = json["jumpStrength"];
        if (json.contains("speed")) c.speed = json["speed"];
        if (json.contains("velocity")) c.velocity = { json["velocity"][0], json["velocity"][1], json["velocity"][2] };
    } else if (componentType == "FootIK") {
        auto& c = registry.get_or_emplace<FootIKComponent>(entity);
        if (json.contains("leftFootOffset")) c.leftFootOffset = { json["leftFootOffset"][0], json["leftFootOffset"][1], json["leftFootOffset"][2] };
        if (json.contains("rightFootOffset")) c.rightFootOffset = { json["rightFootOffset"][0], json["rightFootOffset"][1], json["rightFootOffset"][2] };
        if (json.contains("enabled")) c.enabled = json["enabled"];
    } else if (componentType == "NavAgent") {
        auto& c = registry.get_or_emplace<NavAgentComponent>(entity);
        if (json.contains("target")) c.target = { json["target"][0], json["target"][1], json["target"][2] };
        if (json.contains("velocity")) c.velocity = { json["velocity"][0], json["velocity"][1], json["velocity"][2] };
        if (json.contains("radius")) c.radius = json["radius"];
        if (json.contains("maxSpeed")) c.maxSpeed = json["maxSpeed"];
        if (json.contains("active")) c.active = json["active"];
    } else if (componentType == "SoftBody") {
        auto& c = registry.get_or_emplace<SoftBodyComponent>(entity);
        if (json.contains("mass")) c.mass = json["mass"];
        if (json.contains("pressure")) c.pressure = json["pressure"];
        if (json.contains("enabled")) c.enabled = json["enabled"];
    }
}

void DrawCopyPasteButtons(entt::registry& registry, entt::entity entity, const std::string& componentType) {
    ImGui::PushID(componentType.c_str());
    if (ImGui::Button("Copy JSON")) {
        try {
            nlohmann::json j = CopyComponentToJson(registry, entity, componentType);
            std::string serialized = j.dump(4);
            ImGui::SetClipboardText(serialized.c_str());
            Log::Info("Copied component {} to clipboard.", componentType);
        } catch (...) {
            Log::Error("Failed to copy component {} to clipboard.", componentType);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Paste JSON")) {
        try {
            const char* clip = ImGui::GetClipboardText();
            if (clip) {
                nlohmann::json j = nlohmann::json::parse(clip);
                if (j.contains("ComponentType") && j["ComponentType"] == componentType) {
                    PasteComponentFromJson(registry, entity, j);
                    Log::Info("Pasted component {} from clipboard.", componentType);
                } else {
                    Log::Warn("Clipboard does not contain valid data for component {}.", componentType);
                }
            }
        } catch (const std::exception& e) {
            Log::Error("Failed to paste component: {}", e.what());
        } catch (...) {
            Log::Error("Failed to paste component {} from clipboard.", componentType);
        }
    }
    ImGui::PopID();
}

void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float speed = 0.1f) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
    float lineHeight = ImGui::GetFontSize() + 6;
    ImVec2 buttonSize = ImVec2(lineHeight + 3, lineHeight);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("X", buttonSize)) { values.x = resetValue; }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##x", &values.x, speed, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.9f, 0.2f, 1.0f));
    if (ImGui::Button("Y", buttonSize)) { values.y = resetValue; }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##y", &values.y, speed, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
    if (ImGui::Button("Z", buttonSize)) { values.z = resetValue; }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##z", &values.z, speed, 0.0f, 0.0f, "%.2f");

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

void DrawFloatControl(const std::string& label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    if (min != max) {
        ImGui::DragFloat(("##" + label).c_str(), &value, speed, min, max);
    } else {
        ImGui::DragFloat(("##" + label).c_str(), &value, speed);
    }
    ImGui::Columns(1);
}

void DrawColorControl(const std::string& label, glm::vec4& color) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::ColorEdit4(("##" + label).c_str(), &color.x);
    ImGui::Columns(1);
}

void DrawColor3Control(const std::string& label, glm::vec3& color) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::ColorEdit3(("##" + label).c_str(), &color.x);
    ImGui::Columns(1);
}

void DrawBoolControl(const std::string& label, bool& value) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::Checkbox(("##" + label).c_str(), &value);
    ImGui::Columns(1);
}

void DrawIntControl(const std::string& label, int& value, int speed = 1) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::DragInt(("##" + label).c_str(), &value, (float)speed);
    ImGui::Columns(1);
}

int GetNextNodeId(const std::vector<ModernNode>& nodes) {
    int maxId = 0;
    for (const auto& n : nodes) {
        if (n.id > maxId) maxId = n.id;
    }
    return maxId + 1;
}

std::string ExportNodeLua(int nodeId, int indentLevel, const std::vector<ModernNode>& nodes, const std::vector<ModernLink>& links) {
    std::string indent(indentLevel * 4, ' ');
    const ModernNode* nodePtr = nullptr;
    for (const auto& n : nodes) {
        if (n.id == nodeId) {
            nodePtr = &n;
            break;
        }
    }
    if (!nodePtr) return "";

    std::vector<int> children;
    for (const auto& pin : nodePtr->outputPins) {
        for (const auto& link : links) {
            if (link.fromPin == pin.id) {
                for (const auto& targetNode : nodes) {
                    for (const auto& inPin : targetNode.inputPins) {
                        if (inPin.id == link.toPin) {
                            children.push_back(targetNode.id);
                        }
                    }
                }
            }
        }
    }

    std::string result = indent + "{\n";
    result += indent + "    type = '" + nodePtr->type + "',\n";
    
    std::string cleanName = nodePtr->name;
    if (!cleanName.empty() && cleanName.back() == '?') {
        cleanName.pop_back();
    }
    size_t parenPos = cleanName.find(" (");
    if (parenPos != std::string::npos) {
        cleanName = cleanName.substr(0, parenPos);
    }

    result += indent + "    name = '" + cleanName + "'";

    if (!children.empty()) {
        result += ",\n" + indent + "    children = {\n";
        for (size_t i = 0; i < children.size(); ++i) {
            result += ExportNodeLua(children[i], indentLevel + 2, nodes, links);
            if (i < children.size() - 1) {
                result += ",\n";
            } else {
                result += "\n";
            }
        }
        result += indent + "    }\n";
    } else {
        if (nodePtr->type == "Selector" || nodePtr->type == "Sequence") {
            result += ",\n" + indent + "    children = {}\n";
        } else {
            result += "\n";
        }
    }

    result += indent + "}";
    return result;
}

std::string ResolveShaderExpression(int nodeId, const std::vector<ModernNode>& nodes, const std::vector<ModernLink>& links) {
    const ModernNode* nodePtr = nullptr;
    for (const auto& n : nodes) {
        if (n.id == nodeId) {
            nodePtr = &n;
            break;
        }
    }
    if (!nodePtr) return "vec4(0.0)";

    std::vector<int> inputs;
    for (const auto& pin : nodePtr->inputPins) {
        for (const auto& link : links) {
            if (link.toPin == pin.id) {
                for (const auto& srcNode : nodes) {
                    for (const auto& outPin : srcNode.outputPins) {
                        if (outPin.id == link.fromPin) {
                            inputs.push_back(srcNode.id);
                        }
                    }
                }
            }
        }
    }

    if (nodePtr->type == "Texture") {
        return "texture(uTexture, TexCoord)";
    } else if (nodePtr->type == "Color") {
        char colBuf[128];
        sprintf(colBuf, "vec4(%.3f, %.3f, %.3f, %.3f)", nodePtr->color.Value.x, nodePtr->color.Value.y, nodePtr->color.Value.z, nodePtr->color.Value.w);
        return std::string(colBuf);
    } else if (nodePtr->type == "MathMul") {
        if (inputs.empty()) {
            return "2.5";
        } else if (inputs.size() == 1) {
            std::string expr1 = ResolveShaderExpression(inputs[0], nodes, links);
            return "(" + expr1 + " * 2.5)";
        } else {
            std::string expr1 = ResolveShaderExpression(inputs[0], nodes, links);
            std::string expr2 = ResolveShaderExpression(inputs[1], nodes, links);
            return "(" + expr1 + " * " + expr2 + ")";
        }
    } else if (nodePtr->type == "MathAdd") {
        if (inputs.empty()) {
            return "1.0";
        } else if (inputs.size() == 1) {
            std::string expr1 = ResolveShaderExpression(inputs[0], nodes, links);
            return "(" + expr1 + " + 1.0)";
        } else {
            std::string expr1 = ResolveShaderExpression(inputs[0], nodes, links);
            std::string expr2 = ResolveShaderExpression(inputs[1], nodes, links);
            return "(" + expr1 + " + " + expr2 + ")";
        }
    } else if (nodePtr->type == "Output") {
        if (!inputs.empty()) {
            return ResolveShaderExpression(inputs[0], nodes, links);
        }
        return "vec4(1.0, 1.0, 1.0, 1.0)";
    }
    return "vec4(0.0)";
}

} // anonymous namespace

EditorSystem::~EditorSystem() {
    DestroyViewportFBO();
}

bool EditorSystem::OnInitialize(const EngineContext& context) {
    Log::Info("EditorSystem: Initializing editor framework");

    ApplyDarkTheme();
    ConfigureGlobalIniPath();

    int w = context.window->GetWidth();
    int h = context.window->GetHeight();
    CreateViewportFBO(w, h);

    auto* renderer = context.engine->GetSystem<Renderer>();
    if (renderer) {
        renderer->SetViewportOverride(m_viewportFBO.Get(), m_viewportWidth, m_viewportHeight);
    }

    SetupLuaHotReload();

    m_logicEditorContext = ax::NodeEditor::CreateEditor();
    m_btEditorContext = ax::NodeEditor::CreateEditor();
    m_shaderEditorContext = ax::NodeEditor::CreateEditor();

    // Initialize RmlUi
    s_rmlSystemInterface = std::make_unique<RmlSystemInterfaceSDL>();
    s_rmlRenderInterface = std::make_unique<RmlRenderInterfaceGL3>();
    Rml::SetSystemInterface(s_rmlSystemInterface.get());
    Rml::SetRenderInterface(s_rmlRenderInterface.get());

    if (Rml::Initialise()) {
        s_rmlContext = Rml::CreateContext("main", Rml::Vector2i(w, h));
        if (s_rmlContext) {
            Rml::LoadFontFace(PathResolver::Resolve("assets/fonts/Inconsolata-Regular.ttf"), true);
            Log::Info("EditorSystem: RmlUi initialized successfully.");
        }
    } else {
        Log::Error("EditorSystem: Failed to initialize RmlUi.");
    }

    return true;
}

void EditorSystem::OnShutdown() {
    if (m_logicEditorContext) {
        ax::NodeEditor::DestroyEditor(m_logicEditorContext);
        m_logicEditorContext = nullptr;
    }
    if (m_btEditorContext) {
        ax::NodeEditor::DestroyEditor(m_btEditorContext);
        m_btEditorContext = nullptr;
    }
    if (m_shaderEditorContext) {
        ax::NodeEditor::DestroyEditor(m_shaderEditorContext);
        m_shaderEditorContext = nullptr;
    }
    if (s_rmlContext) {
        Rml::Shutdown();
        s_rmlContext = nullptr;
    }
    s_rmlRenderInterface.reset();
    s_rmlSystemInterface.reset();
}

void EditorSystem::OnUpdate(float dt) {
    if (s_rmlContext) {
        s_rmlContext->Update();
    }
    if (m_mode != EditorMode::Editing) return;

    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    auto& registry = activeScene->GetRegistry();
    auto camView = registry.view<Camera2DComponent, TransformComponent>();
    for (auto entity : camView) {
        auto& cam = camView.get<Camera2DComponent>(entity);
        auto& transform = camView.get<TransformComponent>(entity);
        if (!cam.primary) continue;

        if (m_viewportHovered) {
            float panSpeed = 250.0f / cam.zoom * dt;
            if (InputSystem::IsKeyPressed(pal::KeyCode::W) || InputSystem::IsKeyPressed(pal::KeyCode::Up))
                transform.position.y += panSpeed;
            if (InputSystem::IsKeyPressed(pal::KeyCode::S) || InputSystem::IsKeyPressed(pal::KeyCode::Down))
                transform.position.y -= panSpeed;
            if (InputSystem::IsKeyPressed(pal::KeyCode::A) || InputSystem::IsKeyPressed(pal::KeyCode::Left))
                transform.position.x -= panSpeed;
            if (InputSystem::IsKeyPressed(pal::KeyCode::D) || InputSystem::IsKeyPressed(pal::KeyCode::Right))
                transform.position.x += panSpeed;

            float scroll = ImGui::GetIO().MouseWheel;
            if (scroll != 0.0f) {
                cam.zoom *= (1.0f + scroll * 0.1f);
                cam.zoom = glm::clamp(cam.zoom, 0.1f, 10.0f);
            }
        }
        break;
    }
}

void EditorSystem::DrawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                ImGui::OpenPopup("Save Scene As");
            }
            if (ImGui::MenuItem("Load Scene", "Ctrl+O")) {
                ImGui::OpenPopup("Load Scene");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                SDL_Event quit;
                quit.type = SDL_QUIT;
                SDL_PushEvent(&quit);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hierarchy", "", &m_showHierarchy);
            ImGui::MenuItem("Inspector", "", &m_showInspector);
            ImGui::MenuItem("Content Browser", "", &m_showContentBrowser);
            ImGui::MenuItem("Console", "", &m_showConsole);
            if (ImGui::MenuItem("Visual Shader Editor", "")) {
                m_showUnifiedVisualCoding = true;
                m_activeVisualCodingTab = 2;
            }
            if (ImGui::MenuItem("Visual Behavior Tree Editor", "")) {
                m_showUnifiedVisualCoding = true;
                m_activeVisualCodingTab = 1;
            }
            ImGui::MenuItem("Developer Settings & CVars", "", &m_showCVarEditor);
            ImGui::MenuItem("System Profiler", "", &m_showProfilerWindow);
            if (ImGui::MenuItem("Modern Node Editor", "")) {
                m_showUnifiedVisualCoding = true;
                m_activeVisualCodingTab = 0;
            }
            ImGui::MenuItem("Unified Visual Coding Workspace", "", &m_showUnifiedVisualCoding);
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout", "Ctrl+Shift+Space")) {
                ResetLayout();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Toolbar
    ImGui::Separator();
    bool canPlay = (m_mode == EditorMode::Editing);
    bool canStop = (m_mode != EditorMode::Editing);

    ImGui::BeginDisabled(!canPlay);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
    if (ImGui::Button("Play")) { OnPlay(); }
    ImGui::PopStyleColor(1);
    ImGui::EndDisabled();
    ImGui::SameLine();

    bool canPause = (m_mode == EditorMode::Playing || m_mode == EditorMode::Paused);
    ImGui::BeginDisabled(!canPause);
    const char* pauseLabel = (m_mode == EditorMode::Paused) ? "Resume" : "Pause";
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 0.8f));
    if (ImGui::Button(pauseLabel)) { OnPauseResume(); }
    ImGui::PopStyleColor(1);
    ImGui::EndDisabled();
    ImGui::SameLine();

    ImGui::BeginDisabled(!canStop);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 0.8f));
    if (ImGui::Button("Stop")) { OnStop(); }
    ImGui::PopStyleColor(1);
    ImGui::EndDisabled();

    ImGui::SameLine();
    const char* modeLabels[] = { "Editing", "Playing", "Paused" };
    int modeIdx = static_cast<int>(m_mode);
    ImVec4 modeColor;
    switch (m_mode) {
        case EditorMode::Editing: modeColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break;
        case EditorMode::Playing: modeColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); break;
        case EditorMode::Paused:  modeColor = ImVec4(0.8f, 0.8f, 0.2f, 1.0f); break;
    }
    ImGui::SameLine();
    ImGui::Text("  ");
    ImGui::SameLine();
    ImGui::TextColored(modeColor, "%s", modeLabels[modeIdx]);
}

void EditorSystem::OnPlay() {
    if (m_mode == EditorMode::Playing) return;

    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    auto& registry = activeScene->GetRegistry();
    s_sandboxSnapshot.clear();
    SceneSerializer::SaveToJson(registry, s_sandboxSnapshot);

    m_mode = EditorMode::Playing;
    Log::Info("Sandbox: Play mode started");

    if (s_rmlContext) {
        s_rmlContext->UnloadAllDocuments();
        Rml::ElementDocument* doc = s_rmlContext->LoadDocument("assets/scripts/hud.rml");
        if (doc) {
            doc->Show();
        }
    }
}

void EditorSystem::OnPauseResume() {
    if (m_mode == EditorMode::Playing) {
        m_mode = EditorMode::Paused;
        Engine::Get().SetTimeScale(0.0f);
        Log::Info("Sandbox: Paused");
    } else if (m_mode == EditorMode::Paused) {
        m_mode = EditorMode::Playing;
        Engine::Get().SetTimeScale(1.0f);
        Log::Info("Sandbox: Resumed");
    }
}

void EditorSystem::OnStop() {
    if (m_mode == EditorMode::Editing) return;

    m_mode = EditorMode::Editing;
    Engine::Get().SetTimeScale(1.0f);

    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (activeScene && !s_sandboxSnapshot.empty()) {
        auto& registry = activeScene->GetRegistry();

        // Destruir corpos físicos Jolt antes de limpar a registry para evitar leaks
        auto* physicsSys = Engine::Get().GetSystem<PhysicsSystem>();
        if (physicsSys) {
            auto view = registry.view<PhysicsComponent>();
            for (auto entity : view) {
                physicsSys->DestroyBody(entity);
            }
        }

        registry.clear();
        SceneSerializer::LoadFromJson(registry, s_sandboxSnapshot);
        s_sandboxSnapshot.clear();
        Log::Info("Sandbox: Stopped, snapshot restored");
    }

    if (s_rmlContext) {
        s_rmlContext->UnloadAllDocuments();
    }

    m_selectedEntity = entt::null;
}

void EditorSystem::ApplyDarkTheme() {
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    auto& colors = style.Colors;

    // Obsidian/Violet Dark Outrun palette
    colors[ImGuiCol_WindowBg]          = ImVec4(0.05f, 0.03f, 0.09f, 1.00f);
    colors[ImGuiCol_ChildBg]           = ImVec4(0.07f, 0.04f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg]           = ImVec4(0.05f, 0.03f, 0.09f, 0.96f);
    colors[ImGuiCol_Border]            = ImVec4(1.00f, 0.00f, 0.60f, 0.60f); // Hot Magenta border outline
    colors[ImGuiCol_BorderShadow]      = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]           = ImVec4(0.12f, 0.07f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]    = ImVec4(0.18f, 0.11f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive]     = ImVec4(0.24f, 0.15f, 0.40f, 1.00f);
    colors[ImGuiCol_TitleBg]           = ImVec4(0.04f, 0.02f, 0.08f, 1.00f); // Dark Obsidian
    colors[ImGuiCol_TitleBgActive]     = ImVec4(0.06f, 0.03f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]  = ImVec4(0.04f, 0.02f, 0.08f, 0.75f);
    colors[ImGuiCol_MenuBarBg]         = ImVec4(0.04f, 0.02f, 0.08f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]       = ImVec4(0.04f, 0.02f, 0.08f, 0.50f);
    colors[ImGuiCol_ScrollbarGrab]     = ImVec4(0.55f, 0.12f, 0.75f, 0.70f); // Neon Violet grab
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.12f, 0.75f, 0.90f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(1.00f, 0.00f, 0.60f, 1.00f); // Hot Magenta grab active

    // Accent: Cyber Cyan / Hot Magenta
    ImVec4 accentColor = ImVec4(0.00f, 1.00f, 0.90f, 1.00f); // Cyber Cyan
    ImVec4 accentHover = ImVec4(1.00f, 0.00f, 0.60f, 1.00f); // Hot Magenta
    ImVec4 accentActive = ImVec4(1.00f, 0.45f, 0.05f, 1.00f); // Sunset Orange

    colors[ImGuiCol_CheckMark]         = accentColor;
    colors[ImGuiCol_SliderGrab]        = accentColor;
    colors[ImGuiCol_SliderGrabActive]  = accentActive;
    colors[ImGuiCol_Button]            = ImVec4(0.18f, 0.08f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonHovered]     = ImVec4(1.00f, 0.00f, 0.60f, 0.80f); // Hover: Hot Magenta
    colors[ImGuiCol_ButtonActive]      = ImVec4(0.00f, 1.00f, 0.90f, 0.90f); // Active: Cyber Cyan
    colors[ImGuiCol_Header]            = ImVec4(0.18f, 0.08f, 0.32f, 1.00f);
    colors[ImGuiCol_HeaderHovered]     = ImVec4(1.00f, 0.00f, 0.60f, 0.80f);
    colors[ImGuiCol_HeaderActive]      = ImVec4(0.00f, 1.00f, 0.90f, 0.90f);
    colors[ImGuiCol_Separator]         = ImVec4(0.55f, 0.12f, 0.75f, 0.40f); // Neon Violet separator
    colors[ImGuiCol_SeparatorHovered]  = accentColor;
    colors[ImGuiCol_SeparatorActive]   = accentHover;
    colors[ImGuiCol_Tab]               = ImVec4(0.07f, 0.04f, 0.12f, 1.00f);
    colors[ImGuiCol_TabHovered]        = ImVec4(1.00f, 0.00f, 0.60f, 0.70f);
    colors[ImGuiCol_TabActive]         = ImVec4(0.00f, 1.00f, 0.90f, 1.00f); // Active Tab: Cyber Cyan
    colors[ImGuiCol_TabUnfocused]      = ImVec4(0.05f, 0.03f, 0.09f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.07f, 0.04f, 0.12f, 1.00f);
    colors[ImGuiCol_DockingPreview]    = ImVec4(1.00f, 0.00f, 0.60f, 0.30f);
    colors[ImGuiCol_DockingEmptyBg]    = ImVec4(0.04f, 0.02f, 0.08f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]    = ImVec4(1.00f, 0.00f, 0.60f, 0.25f);
    colors[ImGuiCol_NavHighlight]      = accentActive;

    // Arredondamentos mais amplos e elegantes
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 6.0f;

    // Espaçamentos confortáveis
    style.WindowPadding     = ImVec2(12.0f, 12.0f);
    style.FramePadding      = ImVec2(6.0f, 5.0f);
    style.ItemSpacing       = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f, 4.0f);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 10.0f;

    style.WindowMenuButtonPosition = ImGuiDir_None;
}

void EditorSystem::SetupLuaHotReload() {
    auto* fileWatcher = Engine::Get().GetSystem<FileWatcher>();
    if (!fileWatcher) return;

    auto watchDir = std::filesystem::path(PathResolver::ExeDir()) / "assets" / "scripts";
    if (!std::filesystem::exists(watchDir)) return;

    for (auto& entry : std::filesystem::directory_iterator(watchDir)) {
        if (entry.path().extension() == ".lua") {
            std::string path = entry.path().string();
            m_watchedLuaScripts.push_back(path);
            fileWatcher->AddWatch(path, [](const std::string& changedPath) {
                Log::Info("Hot-Reload: Lua script changed, re-executing: " + changedPath);
                auto* scripting = Engine::Get().GetSystem<ScriptSystem>();
                if (scripting) {
                    scripting->ExecuteFile(changedPath);
                }
            });
        }
    }

    if (!m_watchedLuaScripts.empty()) {
        Log::Info("EditorSystem: Lua hot-reload active for " + std::to_string(m_watchedLuaScripts.size()) + " scripts");
    }
}

void EditorSystem::DrawGizmos(entt::registry& registry, const ImVec2& viewportPos) {
    if (m_mode != EditorMode::Editing) return;
    if (m_selectedEntity == entt::null) return;
    if (!registry.all_of<TransformComponent>(m_selectedEntity)) return;

    auto& tc = registry.get<TransformComponent>(m_selectedEntity);
    ImVec2 viewportMin = viewportPos;
    ImVec2 viewportSize = ImVec2((float)m_viewportWidth, (float)m_viewportHeight);

    // Calculate camera info for world-to-screen mapping
    float camZoom = 1.0f;
    glm::vec2 camPos = {0.0f, 0.0f};
    auto camView = registry.view<Camera2DComponent, TransformComponent>();
    for (auto camEntity : camView) {
        auto& cam = camView.get<Camera2DComponent>(camEntity);
        if (cam.primary) {
            camZoom = cam.zoom;
            auto& camTransform = camView.get<TransformComponent>(camEntity);
            camPos = {camTransform.position.x, camTransform.position.y};
            break;
        }
    }

    float aspect = (float)m_viewportWidth / (float)m_viewportHeight;
    float halfWidth = (float)m_viewportWidth * 0.5f / camZoom;
    float halfHeight = (float)m_viewportHeight * 0.5f / camZoom;
    if (aspect > 1.0f) {
        halfWidth *= aspect;
    } else {
        halfHeight /= aspect;
    }

    // World -> Screen
    auto worldToScreen = [&](const glm::vec2& world) -> ImVec2 {
        float sx = (world.x - camPos.x + halfWidth) / (halfWidth * 2.0f) * (float)m_viewportWidth;
        float sy = (1.0f - (world.y - camPos.y + halfHeight) / (halfHeight * 2.0f)) * (float)m_viewportHeight;
        return ImVec2(viewportMin.x + sx, viewportMin.y + sy);
    };

    // Screen -> World
    auto screenToWorld = [&](const ImVec2& screen) -> glm::vec2 {
        float sx = screen.x - viewportMin.x;
        float sy = screen.y - viewportMin.y;
        float wx = (sx / (float)m_viewportWidth) * halfWidth * 2.0f - halfWidth + camPos.x;
        float wy = (1.0f - sy / (float)m_viewportHeight) * halfHeight * 2.0f - halfHeight + camPos.y;
        return {wx, wy};
    };

    glm::vec2 entityPos2D = {tc.position.x, tc.position.y};
    ImVec2 screenPos = worldToScreen(entityPos2D);

    // Clamp to viewport bounds
    if (screenPos.x < viewportMin.x || screenPos.x > viewportMin.x + (float)m_viewportWidth ||
        screenPos.y < viewportMin.y || screenPos.y > viewportMin.y + (float)m_viewportHeight) {
        return;
    }

    // Draw the gizmo handle
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float handleRadius = 7.0f;
    ImU32 handleColor = IM_COL32(255, 200, 50, 220);
    ImU32 handleColorHover = IM_COL32(255, 230, 100, 255);
    ImU32 handleOutline = IM_COL32(0, 0, 0, 180);

    ImVec2 handleMin(screenPos.x - handleRadius, screenPos.y - handleRadius);
    ImVec2 handleMax(screenPos.x + handleRadius, screenPos.y + handleRadius);

    bool hovered = ImGui::IsMouseHoveringRect(handleMin, handleMax) && m_viewportHovered;

    // Draw crosshair
    drawList->AddLine(
        ImVec2(screenPos.x - handleRadius * 1.5f, screenPos.y),
        ImVec2(screenPos.x + handleRadius * 1.5f, screenPos.y),
        hovered ? handleColorHover : handleColor, 2.0f
    );
    drawList->AddLine(
        ImVec2(screenPos.x, screenPos.y - handleRadius * 1.5f),
        ImVec2(screenPos.x, screenPos.y + handleRadius * 1.5f),
        hovered ? handleColorHover : handleColor, 2.0f
    );
    drawList->AddCircleFilled(screenPos, 4.0f, hovered ? handleColorHover : handleColor);
    drawList->AddCircle(screenPos, handleRadius, handleOutline, 0, 2.0f);

    // Dragging logic
    if (hovered && ImGui::IsMouseClicked(0)) {
        m_gizmoDragging = true;
        m_gizmoDragStartX = ImGui::GetMousePos().x;
        m_gizmoDragStartY = ImGui::GetMousePos().y;
        m_gizmoEntityStartPos = tc.position;
    }

    if (m_gizmoDragging) {
        if (ImGui::IsMouseDragging(0, 0.0f)) {
            ImVec2 currentMouse = ImGui::GetMousePos();
            ImVec2 deltaScreen(currentMouse.x - m_gizmoDragStartX, currentMouse.y - m_gizmoDragStartY);
            ImVec2 refScreen(m_gizmoDragStartX + deltaScreen.x, m_gizmoDragStartY + deltaScreen.y);
            glm::vec2 currentWorld = screenToWorld(refScreen);
            glm::vec2 startWorld = screenToWorld(ImVec2(m_gizmoDragStartX, m_gizmoDragStartY));
            glm::vec2 deltaWorld = currentWorld - startWorld;
            tc.position.x = m_gizmoEntityStartPos.x + deltaWorld.x;
            tc.position.y = m_gizmoEntityStartPos.y + deltaWorld.y;
        } else {
            m_gizmoDragging = false;
        }
    }

    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    // Draw ReverbZone visual boundaries if the entity has ReverbZoneComponent
    if (registry.all_of<ReverbZoneComponent>(m_selectedEntity)) {
        auto& rzc = registry.get<ReverbZoneComponent>(m_selectedEntity);
        if (rzc.active) {
            float minRadiusScreen = rzc.minDistance * camZoom;
            float maxRadiusScreen = rzc.maxDistance * camZoom;
            
            // Draw neon green circle for minDistance
            drawList->AddCircle(screenPos, minRadiusScreen, IM_COL32(50, 255, 50, 200), 64, 2.0f);
            drawList->AddText(ImVec2(screenPos.x + 5.0f, screenPos.y - minRadiusScreen - 15.0f), IM_COL32(50, 255, 50, 255), "Reverb Min");
            
            // Draw neon blue/cyan circle for maxDistance
            drawList->AddCircle(screenPos, maxRadiusScreen, IM_COL32(0, 191, 255, 200), 64, 2.0f);
            drawList->AddText(ImVec2(screenPos.x + 5.0f, screenPos.y - maxRadiusScreen - 15.0f), IM_COL32(0, 191, 255, 255), "Reverb Max");
        }
    }
}

void EditorSystem::CreateViewportFBO(int width, int height) {
    m_viewportFBO.Destroy();
    m_viewportTex.Destroy();
    m_viewportDepth.Destroy();

    width = (width < 1) ? 1 : width;
    height = (height < 1) ? 1 : height;

    glGenFramebuffers(1, m_viewportFBO.Ptr());
    m_viewportFBO.Bind();

    glGenTextures(1, m_viewportTex.Ptr());
    m_viewportTex.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_viewportTex.Get(), 0);

    glGenRenderbuffers(1, m_viewportDepth.Ptr());
    m_viewportDepth.Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_viewportDepth.Get());

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        Log::Error("EditorSystem: Viewport FBO incomplete (status: {})", (int)status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_viewportWidth = width;
    m_viewportHeight = height;
}

void EditorSystem::DestroyViewportFBO() {
    m_viewportFBO.Destroy();
    m_viewportTex.Destroy();
    m_viewportDepth.Destroy();
}

void EditorSystem::OnRender() {
    auto* renderer = Engine::Get().GetSystem<Renderer>();

    // Main Dockspace
    ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    dockspaceFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    dockspaceFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    
    ImGui::Begin("DockSpace", nullptr, dockspaceFlags);
    ImGui::PopStyleVar(3); // Pop rounding, border, padding

    ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Space)) {
            ResetLayout();
        }
    }

    if (!m_layoutInitialized) {
        BuildDefaultDockLayout(dockspaceID);
    }

    auto activeScene = Engine::Get().GetSceneStack().Active();
    entt::registry* registryPtr = activeScene ? &activeScene->GetRegistry() : nullptr;

    DrawMenuBar();

    // === Viewport Panel ===
    ImGui::Begin("Viewport");
    m_viewportHovered = ImGui::IsWindowHovered();
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    if (contentSize.x > 0 && contentSize.y > 0) {
        int newW = (int)contentSize.x;
        int newH = (int)contentSize.y;
        if (newW != m_viewportWidth || newH != m_viewportHeight) {
            CreateViewportFBO(newW, newH);
            if (renderer) {
                renderer->SetViewportOverride(m_viewportFBO.Get(), m_viewportWidth, m_viewportHeight);
            }
            if (s_rmlContext) {
                s_rmlContext->SetDimensions(Rml::Vector2i(newW, newH));
            }
        }

        if (m_viewportTex) {
            if (m_mode == EditorMode::Playing && s_rmlContext) {
                GLint last_fbo = 0;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING, &last_fbo);
                m_viewportFBO.Bind();
                glViewport(0, 0, m_viewportWidth, m_viewportHeight);
                s_rmlContext->Render();
                glBindFramebuffer(GL_FRAMEBUFFER, last_fbo);
            }

            ImVec2 textureScreenPos = ImGui::GetCursorScreenPos();
            m_viewportScreenPosX = textureScreenPos.x;
            m_viewportScreenPosY = textureScreenPos.y;
            ImGui::Image((ImTextureID)(uint64_t)(uintptr_t)m_viewportTex.Get(), contentSize, ImVec2(0, 1), ImVec2(1, 0));

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    const char* filepath = (const char*)payload->Data;
                    fs::path p(filepath);
                    if (p.extension() == ".scene") {
                        if (activeScene) {
                            if (m_mode != EditorMode::Editing) {
                                OnStop();
                            }
                            SceneSerializer::LoadFromFile(*activeScene, p.filename().string());
                            m_sceneFilePath = p.filename().string();
                            Log::Info("Scene loaded via drag-and-drop on Viewport: {}", m_sceneFilePath);
                        }
                    } else if (p.extension() == ".json") {
                        if (registryPtr) {
                            PrefabSystem::Instantiate(filepath, registryPtr);
                            Log::Info("Prefab instantiated via drag-and-drop on Viewport: {}", p.filename().string());
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (registryPtr) {
                DrawGizmos(*registryPtr, textureScreenPos);
            }

            // Desenhar Overlay de Estatísticas (Viewport Overlay) se r_showProfiler > 0
            auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
            int showProfiler = cvarSys ? cvarSys->GetInt("r_showProfiler") : 0;
            
            if (showProfiler > 0) {
                ImVec2 viewportPos = ImGui::GetWindowPos();
                // Deslocar para baixo da barra de título da janela
                ImGui::SetNextWindowPos(ImVec2(viewportPos.x + 10.0f, viewportPos.y + 35.0f));
                ImGui::SetNextWindowBgAlpha(0.75f); // Fundo semi-transparente
                ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | 
                                                ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | 
                                                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
                
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.02f, 0.08f, 0.90f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.6f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));

                if (ImGui::Begin("##ViewportStats", nullptr, overlayFlags)) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "STARLIGHT PERFORMANCE HUD");
                    ImGui::Separator();
                    
                    const char* glVendor = (const char*)glGetString(GL_VENDOR);
                    const char* glRenderer = (const char*)glGetString(GL_RENDERER);
                    if (glRenderer) {
                        if (glVendor) {
                            ImGui::Text("GPU: %s (%s)", glRenderer, glVendor);
                        } else {
                            ImGui::Text("GPU: %s", glRenderer);
                        }
                    }
                    
                    ImGui::Text("FPS: %.1f (%.3f ms)", Engine::Get().GetTime().fps, Engine::Get().GetTime().avgFrameTime * 1000.0f);
                    
                    auto stats = Renderer2D::GetStats();
                    ImGui::Text("Draw Calls: %d | Quads: %d", stats.drawCalls, stats.quadCount);
                    
                    if (registryPtr) {
                        size_t entCount = registryPtr->storage<entt::entity>().size();
                        ImGui::Text("Entities: %zu", entCount);
                    }

                    if (showProfiler == 2) {
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "CPU SUBSYSTEMS LATENCY");
                        ImGui::Separator();
                        
                        const auto& prof = Engine::Get().GetProfilerStats();
                        
                        auto drawSubsystemTime = [](const char* label, float timeMs, const ImVec4& color) {
                            ImGui::Text("%-8s: %6.2f ms", label, timeMs);
                            ImGui::SameLine();
                            float progress = timeMs / 16.67f;
                            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
                            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                            ImGui::ProgressBar(progress, ImVec2(100.0f, ImGui::GetTextLineHeight() * 0.8f), "");
                            ImGui::PopStyleColor(2);
                        };
                        
                        drawSubsystemTime("Render", prof.renderTime, ImVec4(0.0f, 1.0f, 0.9f, 1.0f));
                        drawSubsystemTime("Scripts", prof.scriptTime, ImVec4(1.0f, 0.0f, 0.6f, 1.0f));
                        drawSubsystemTime("Physics", prof.physicsTime, ImVec4(1.0f, 0.7f, 0.0f, 1.0f));
                        drawSubsystemTime("Audio", prof.audioTime, ImVec4(0.2f, 1.0f, 0.4f, 1.0f));
                        drawSubsystemTime("Update", prof.updateTime, ImVec4(0.7f, 0.4f, 1.0f, 1.0f));
                        
                        static float fpsHistory[100] = {0};
                        static int fpsIndex = 0;
                        fpsHistory[fpsIndex] = Engine::Get().GetTime().fps;
                        fpsIndex = (fpsIndex + 1) % 100;
                        
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "FPS HISTORY (100 frames)");
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.09f, 0.04f, 0.16f, 0.5f));
                        ImGui::PlotLines("##FPSPlot", fpsHistory, 100, fpsIndex, nullptr, 0.0f, 120.0f, ImVec2(220.0f, 40.0f));
                        ImGui::PopStyleColor();
                    }
                }
                ImGui::End();
                ImGui::PopStyleColor(3);
            }
        }
    }
    ImGui::End();

    // === Hierarchy Panel ===
    if (m_showHierarchy) {
        ImGui::Begin("Hierarchy", &m_showHierarchy);
        
        ImGui::InputTextWithHint("##HierarchySearch", "Search Entity...", m_hierarchySearch, IM_ARRAYSIZE(m_hierarchySearch));
        ImGui::Separator();

        if (registryPtr) {
            auto& entityStorage = registryPtr->storage<entt::entity>();
            std::vector<entt::entity> toDelete;
            for (auto entityIter = entityStorage.begin(); entityIter != entityStorage.end(); ++entityIter) {
                entt::entity e = *entityIter;
                bool isSelected = (e == m_selectedEntity);
                std::string name = "Entity " + std::to_string((uint32_t)e);
                if (registryPtr->all_of<TagComponent>(e)) {
                    name = registryPtr->get<TagComponent>(e).tag;
                }

                if (m_hierarchySearch[0] != '\0') {
                    if (!ContainsCaseInsensitive(name, m_hierarchySearch) &&
                        !ContainsCaseInsensitive(std::to_string((uint32_t)e), m_hierarchySearch)) {
                        continue;
                    }
                }

                ImGui::PushID((void*)(uint64_t)(uint32_t)e);
                if (ImGui::Selectable(name.c_str(), &isSelected)) {
                    m_selectedEntity = e;
                }
                // Right-click context menu (only in Editing mode)
                bool isEditing = (m_mode == EditorMode::Editing);
                if (isEditing && ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete Entity")) {
                        toDelete.push_back(e);
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }
            // Deferred deletion to avoid iterator invalidation
            for (auto e : toDelete) {
                auto* physicsSys = Engine::Get().GetSystem<PhysicsSystem>();
                if (physicsSys) {
                    physicsSys->DestroyBody(e);
                }
                registryPtr->destroy(e);
                if (m_selectedEntity == e) m_selectedEntity = entt::null;
            }

            // Drop target on Hierarchy panel
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    const char* filepath = (const char*)payload->Data;
                    fs::path p(filepath);
                    if (p.extension() == ".json") {
                        PrefabSystem::Instantiate(filepath, registryPtr);
                        Log::Info("Prefab instantiated via drag-and-drop: {}", p.filename().string());
                    } else if (p.extension() == ".scene") {
                        if (activeScene) {
                            if (m_mode != EditorMode::Editing) {
                                OnStop();
                            }
                            SceneSerializer::LoadFromFile(*activeScene, p.filename().string());
                            m_sceneFilePath = p.filename().string();
                            Log::Info("Scene loaded via drag-and-drop: {}", m_sceneFilePath);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // Right-click on empty space (only in Editing mode)
            if (m_mode == EditorMode::Editing && ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                if (ImGui::MenuItem("Create Empty Entity")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity);
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Sprite Entity")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<SpriteComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "Sprite");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Camera 2D")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<Camera2DComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "Camera2D");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Animated Sprite")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<SpriteComponent>(newEntity);
                    registryPtr->emplace<SpriteAnimationComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "AnimatedSprite");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Point Light")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<PointLightComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "PointLight");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create 3D Mesh")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<MeshComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "Mesh");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Reverb Zone")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<ReverbZoneComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "ReverbZone");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Retro Mode7")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<RetroComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "RetroMode7");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Instantiate Prefab...")) {
                    ImGui::OpenPopup("InstantiatePrefabModal");
                }
                ImGui::EndPopup();
            }
        }

        // Instantiate Prefab Modal Dialog
        if (ImGui::BeginPopupModal("InstantiatePrefabModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char prefabPathBuf[256] = "";
            ImGui::Text("Prefab JSON File Path:");
            ImGui::InputText("##prefabpath", prefabPathBuf, IM_ARRAYSIZE(prefabPathBuf));
            
            if (ImGui::Button("Instantiate", ImVec2(120, 0))) {
                if (registryPtr) {
                    PrefabSystem::Instantiate(prefabPathBuf, registryPtr);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    // === Inspector Panel ===
    if (m_showInspector) {
        ImGui::Begin("Inspector", &m_showInspector);
        if (registryPtr && m_selectedEntity != entt::null) {
            entt::entity selected = m_selectedEntity;
            bool isReadOnly = (m_mode != EditorMode::Editing);

        // Tag
        if (registryPtr->all_of<TagComponent>(selected)) {
            auto& tag = registryPtr->get<TagComponent>(selected).tag;
            char buf[256];
            strcpy_s(buf, sizeof(buf), tag.c_str());
            if (isReadOnly) {
                ImGui::Text("Tag: %s", tag.c_str());
            } else {
                ImGui::SetNextItemWidth(180);
                if (ImGui::InputText("Tag", buf, sizeof(buf))) {
                    tag = buf;
                }
                ImGui::SameLine();
                if (ImGui::Button("Save as Prefab")) {
                    ImGui::OpenPopup("SavePrefabModal");
                }
            }
        }

        if (ImGui::BeginPopupModal("SavePrefabModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char prefabPathBuf[256] = "";
            if (prefabPathBuf[0] == '\0') {
                auto& tag = registryPtr->get<TagComponent>(selected).tag;
                sprintf_s(prefabPathBuf, "assets/prefabs/%s.json", tag.c_str());
            }
            ImGui::Text("Save Prefab JSON File to Path:");
            ImGui::InputText("##saveprefabpath", prefabPathBuf, IM_ARRAYSIZE(prefabPathBuf));
            
            if (ImGui::Button("Save", ImVec2(120, 0))) {
                std::filesystem::path p(prefabPathBuf);
                std::filesystem::create_directories(p.parent_path());
                PrefabSystem::Save(prefabPathBuf, selected, registryPtr);
                ImGui::CloseCurrentPopup();
                prefabPathBuf[0] = '\0';
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                prefabPathBuf[0] = '\0';
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Text("Components");
        ImGui::Separator();

        ImGui::BeginDisabled(isReadOnly);

        // TransformComponent
        if (registryPtr->all_of<TransformComponent>(selected)) {
            auto& tc = registryPtr->get<TransformComponent>(selected);
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Transform");
                ImGui::Separator();
                if (ImGui::Button("Reset Transform")) {
                    tc.position = glm::vec3(0.0f);
                    tc.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                    tc.scale = glm::vec3(1.0f);
                }
                ImGui::Separator();
                DrawVec3Control("Position", tc.position, 0.0f);
                glm::vec3 euler = glm::eulerAngles(tc.rotation);
                glm::vec3 eulerDeg = glm::degrees(euler);
                DrawVec3Control("Rotation", eulerDeg, 0.0f);
                tc.rotation = glm::quat(glm::radians(eulerDeg));
                DrawVec3Control("Scale", tc.scale, 1.0f);
                ImGui::Unindent(8);
            }
        }

        // SpriteComponent
        if (registryPtr->all_of<SpriteComponent>(selected)) {
            auto& sc = registryPtr->get<SpriteComponent>(selected);
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Sprite");
                ImGui::Separator();
                if (ImGui::Button("Remove Sprite Component")) {
                    registryPtr->remove<SpriteComponent>(selected);
                } else {
                    DrawColorControl("Color", sc.color);
                    DrawFloatControl("Pivot X", sc.pivot.x, 0.01f, 0.0f, 1.0f);
                    DrawFloatControl("Pivot Y", sc.pivot.y, 0.01f, 0.0f, 1.0f);
                    DrawIntControl("Layer", sc.layer);
                    DrawIntControl("Order", sc.orderInLayer);
                    DrawBoolControl("Flip X", sc.flipX);
                    DrawBoolControl("Flip Y", sc.flipY);
                    DrawBoolControl("Visible", sc.visible);
                }
                ImGui::Unindent(8);
            }
        }

        // Camera2DComponent
        if (registryPtr->all_of<Camera2DComponent>(selected)) {
            auto& cc = registryPtr->get<Camera2DComponent>(selected);
            if (ImGui::CollapsingHeader("Camera 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Camera2D");
                ImGui::Separator();
                if (ImGui::Button("Remove Camera 2D")) {
                    registryPtr->remove<Camera2DComponent>(selected);
                } else {
                    DrawFloatControl("Zoom", cc.zoom, 0.1f, 0.01f, 100.0f);
                    DrawFloatControl("Rotation", cc.rotation, 0.5f);
                    DrawBoolControl("Primary", cc.primary);
                }
                ImGui::Unindent(8);
            }
        }

        // SpriteAnimationComponent
        if (registryPtr->all_of<SpriteAnimationComponent>(selected)) {
            auto& ac = registryPtr->get<SpriteAnimationComponent>(selected);
            if (ImGui::CollapsingHeader("Sprite Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "SpriteAnimation");
                ImGui::Separator();
                if (ImGui::Button("Remove Sprite Animation")) {
                    registryPtr->remove<SpriteAnimationComponent>(selected);
                } else {
                    ImGui::Text("Frame: %d / %d", ac.currentFrame + 1, (int)ac.frames.size());
                    DrawFloatControl("FPS", ac.fps, 0.5f, 0.1f, 120.0f);
                    DrawBoolControl("Playing", ac.playing);
                    DrawBoolControl("Looping", ac.looping);
                    if (ImGui::Button("Reset")) {
                        ac.currentFrame = 0;
                        ac.timer = 0.0f;
                    }
                }
                ImGui::Unindent(8);
            }
        }

        // TilemapComponent
        if (registryPtr->all_of<TilemapComponent>(selected)) {
            auto& tc = registryPtr->get<TilemapComponent>(selected);
            if (ImGui::CollapsingHeader("Tilemap", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Tilemap");
                ImGui::Separator();
                if (ImGui::Button("Remove Tilemap Component")) {
                    registryPtr->remove<TilemapComponent>(selected);
                } else {
                    ImGui::Text("Map: %d x %d", tc.mapWidth, tc.mapHeight);
                    DrawIntControl("Tile Size", tc.tileSize);
                    DrawIntControl("Tileset Cols", tc.tilesetColumns);
                    DrawIntControl("Tileset Rows", tc.tilesetRows);
                    DrawColorControl("Color", tc.color);
                    DrawIntControl("Layer", tc.layer);
                }
                ImGui::Unindent(8);
            }
        }

        // MeshComponent (3D)
        if (registryPtr->all_of<MeshComponent>(selected)) {
            auto& mc = registryPtr->get<MeshComponent>(selected);
            if (ImGui::CollapsingHeader("Mesh Renderer (3D)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Mesh");
                ImGui::Separator();
                if (ImGui::Button("Remove Mesh Component")) {
                    registryPtr->remove<MeshComponent>(selected);
                } else {
                    ImGui::Text("Material Settings:");
                    DrawColor3Control("Albedo", mc.material.albedo);
                    DrawFloatControl("Metallic", mc.material.metallic, 0.01f, 0.0f, 1.0f);
                    DrawFloatControl("Roughness", mc.material.roughness, 0.01f, 0.0f, 1.0f);
                    DrawBoolControl("PBR Enabled", mc.material.isPBR);
                    DrawBoolControl("Visible", mc.isVisible);
                }
                ImGui::Unindent(8);
            }
        }

        // PointLightComponent (3D)
        if (registryPtr->all_of<PointLightComponent>(selected)) {
            auto& plc = registryPtr->get<PointLightComponent>(selected);
            if (ImGui::CollapsingHeader("Point Light (3D)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "PointLight");
                ImGui::Separator();
                if (ImGui::Button("Remove Point Light")) {
                    registryPtr->remove<PointLightComponent>(selected);
                } else {
                    DrawColor3Control("Color", plc.color);
                    DrawFloatControl("Intensity", plc.intensity, 1.0f, 0.0f, 10000.0f);
                }
                ImGui::Unindent(8);
            }
        }

        // RetroComponent (Mode-7)
        if (registryPtr->all_of<RetroComponent>(selected)) {
            auto& rc = registryPtr->get<RetroComponent>(selected);
            if (ImGui::CollapsingHeader("Retro Mode-7 Component", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Retro");
                ImGui::Separator();
                if (ImGui::Button("Remove Retro Component")) {
                    registryPtr->remove<RetroComponent>(selected);
                } else {
                    DrawFloatControl("Map X", rc.map_x, 0.1f);
                    DrawFloatControl("Map Y", rc.map_y, 0.1f);
                    DrawFloatControl("Map Z", rc.map_z, 0.01f);
                    DrawFloatControl("Horizon", rc.horizon, 0.01f);
                    DrawFloatControl("Pitch", rc.pitch, 0.01f);
                    DrawColor3Control("Sky Color", rc.skyColor);
                    DrawBoolControl("Active", rc.active);
                }
                ImGui::Unindent(8);
            }
        }

        // PhysicsComponent (Jolt)
        if (registryPtr->all_of<PhysicsComponent>(selected)) {
            if (ImGui::CollapsingHeader("Physics Body (Jolt)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                if (ImGui::Button("Remove Physics Component")) {
                    registryPtr->remove<PhysicsComponent>(selected);
                } else {
                    ImGui::Text("Body Registered in Jolt Physics.");
                    ImGui::Text("Body ID: %u", registryPtr->get<PhysicsComponent>(selected).bodyID.GetIndexAndSequenceNumber());
                }
                ImGui::Unindent(8);
            }
        }

        // CameraComponent (3D)
        if (registryPtr->all_of<CameraComponent>(selected)) {
            auto& cc = registryPtr->get<CameraComponent>(selected);
            if (ImGui::CollapsingHeader("Camera 3D", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "Camera3D");
                ImGui::Separator();
                if (ImGui::Button("Remove Camera 3D")) {
                    registryPtr->remove<CameraComponent>(selected);
                } else {
                    DrawFloatControl("FOV", cc.fov, 0.5f, 10.0f, 120.0f);
                    DrawFloatControl("Near Plane", cc.nearPlane, 0.01f, 0.01f, 10.0f);
                    DrawFloatControl("Far Plane", cc.farPlane, 1.0f, 10.0f, 5000.0f);
                    DrawBoolControl("Primary", cc.primary);
                }
                ImGui::Unindent(8);
            }
        }

        // ReverbZoneComponent
        if (registryPtr->all_of<ReverbZoneComponent>(selected)) {
            auto& rzc = registryPtr->get<ReverbZoneComponent>(selected);
            if (ImGui::CollapsingHeader("Reverb Zone", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "ReverbZone");
                ImGui::Separator();
                if (ImGui::Button("Remove Reverb Zone Component")) {
                    registryPtr->remove<ReverbZoneComponent>(selected);
                } else {
                    DrawFloatControl("Min Distance", rzc.minDistance, 0.5f, 0.0f, 1000.0f);
                    DrawFloatControl("Max Distance", rzc.maxDistance, 0.5f, 0.0f, 1000.0f);
                    DrawFloatControl("Reverb Factor", rzc.reverbFactor, 0.05f, 0.0f, 1.0f);
                    DrawBoolControl("Active", rzc.active);
                }
                ImGui::Unindent(8);
            }
        }

        // CharacterControllerComponent
        if (registryPtr->all_of<CharacterControllerComponent>(selected)) {
            auto& c = registryPtr->get<CharacterControllerComponent>(selected);
            if (ImGui::CollapsingHeader("Character Controller (Physics)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "CharacterController");
                ImGui::Separator();
                if (ImGui::Button("Remove Character Controller Component")) {
                    registryPtr->remove<CharacterControllerComponent>(selected);
                } else {
                    DrawFloatControl("Height", c.height, 0.1f, 0.1f, 10.0f);
                    DrawFloatControl("Radius", c.radius, 0.05f, 0.05f, 5.0f);
                    DrawFloatControl("Max Slope Angle", c.maxSlopeAngle, 1.0f, 0.0f, 90.0f);
                    DrawFloatControl("Jump Strength", c.jumpStrength, 0.2f, 0.0f, 100.0f);
                    DrawFloatControl("Speed", c.speed, 0.2f, 0.0f, 100.0f);
                    DrawVec3Control("Velocity", c.velocity);
                    ImGui::Text("Grounded: %s", c.isGrounded ? "Yes" : "No");
                }
                ImGui::Unindent(8);
            }
        }

        // FootIKComponent
        if (registryPtr->all_of<FootIKComponent>(selected)) {
            auto& f = registryPtr->get<FootIKComponent>(selected);
            if (ImGui::CollapsingHeader("Foot IK (Animation)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "FootIK");
                ImGui::Separator();
                if (ImGui::Button("Remove Foot IK Component")) {
                    registryPtr->remove<FootIKComponent>(selected);
                } else {
                    DrawVec3Control("Left Foot Offset", f.leftFootOffset);
                    DrawVec3Control("Right Foot Offset", f.rightFootOffset);
                    DrawBoolControl("Enabled", f.enabled);
                }
                ImGui::Unindent(8);
            }
        }

        // NavAgentComponent
        if (registryPtr->all_of<NavAgentComponent>(selected)) {
            auto& n = registryPtr->get<NavAgentComponent>(selected);
            if (ImGui::CollapsingHeader("Nav Agent (AI)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "NavAgent");
                ImGui::Separator();
                if (ImGui::Button("Remove Nav Agent Component")) {
                    registryPtr->remove<NavAgentComponent>(selected);
                } else {
                    DrawVec3Control("Target", n.target);
                    DrawVec3Control("Velocity", n.velocity);
                    DrawFloatControl("Radius", n.radius, 0.05f, 0.01f, 10.0f);
                    DrawFloatControl("Max Speed", n.maxSpeed, 0.1f, 0.0f, 50.0f);
                    DrawBoolControl("Active", n.active);
                }
                ImGui::Unindent(8);
            }
        }

        // SoftBodyComponent
        if (registryPtr->all_of<SoftBodyComponent>(selected)) {
            auto& s = registryPtr->get<SoftBodyComponent>(selected);
            if (ImGui::CollapsingHeader("Soft Body (Physics)", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawCopyPasteButtons(*registryPtr, selected, "SoftBody");
                ImGui::Separator();
                if (ImGui::Button("Remove Soft Body Component")) {
                    registryPtr->remove<SoftBodyComponent>(selected);
                } else {
                    DrawFloatControl("Mass", s.mass, 0.1f, 0.001f, 1000.0f);
                    DrawFloatControl("Pressure", s.pressure, 0.1f, 0.0f, 1000.0f);
                    DrawBoolControl("Enabled", s.enabled);
                }
                ImGui::Unindent(8);
            }
        }

        // Add Component Menu
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Add Component...", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f))) {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup")) {
            if (!registryPtr->all_of<SpriteComponent>(selected) && ImGui::MenuItem("Sprite Component")) {
                registryPtr->emplace<SpriteComponent>(selected);
            }
            if (!registryPtr->all_of<Camera2DComponent>(selected) && ImGui::MenuItem("Camera 2D Component")) {
                registryPtr->emplace<Camera2DComponent>(selected);
            }
            if (!registryPtr->all_of<SpriteAnimationComponent>(selected) && ImGui::MenuItem("Sprite Animation")) {
                registryPtr->emplace<SpriteAnimationComponent>(selected);
            }
            if (!registryPtr->all_of<TilemapComponent>(selected) && ImGui::MenuItem("Tilemap Component")) {
                registryPtr->emplace<TilemapComponent>(selected);
            }
            if (!registryPtr->all_of<MeshComponent>(selected) && ImGui::MenuItem("Mesh Renderer (3D)")) {
                auto& mc = registryPtr->emplace<MeshComponent>(selected);
                mc.material.isPBR = true;
                mc.mesh = Engine::Get().GetRenderer().GetCubeMesh();
            }
            if (!registryPtr->all_of<PointLightComponent>(selected) && ImGui::MenuItem("Point Light (3D)")) {
                registryPtr->emplace<PointLightComponent>(selected);
            }
            if (!registryPtr->all_of<RetroComponent>(selected) && ImGui::MenuItem("Retro Mode-7 Component")) {
                registryPtr->emplace<RetroComponent>(selected);
            }
            if (!registryPtr->all_of<CameraComponent>(selected) && ImGui::MenuItem("Camera 3D")) {
                registryPtr->emplace<CameraComponent>(selected);
            }
            if (!registryPtr->all_of<ReverbZoneComponent>(selected) && ImGui::MenuItem("Reverb Zone Component")) {
                registryPtr->emplace<ReverbZoneComponent>(selected);
            }
            if (!registryPtr->all_of<CharacterControllerComponent>(selected) && ImGui::MenuItem("Character Controller Component")) {
                registryPtr->emplace<CharacterControllerComponent>(selected);
            }
            if (!registryPtr->all_of<FootIKComponent>(selected) && ImGui::MenuItem("Foot IK Component")) {
                registryPtr->emplace<FootIKComponent>(selected);
            }
            if (!registryPtr->all_of<NavAgentComponent>(selected) && ImGui::MenuItem("Nav Agent Component")) {
                registryPtr->emplace<NavAgentComponent>(selected);
            }
            if (!registryPtr->all_of<SoftBodyComponent>(selected) && ImGui::MenuItem("Soft Body Component")) {
                registryPtr->emplace<SoftBodyComponent>(selected);
            }
            ImGui::EndPopup();
        }

            ImGui::EndDisabled();
        } else {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No entity selected.");
            ImGui::TextWrapped("Select an entity in the Hierarchy panel to view and edit its components.");
        }
        ImGui::End();
    }

    // === Content Browser Panel ===
    if (m_showContentBrowser) {
        ImGui::Begin("Content Browser", &m_showContentBrowser);
        static fs::path currentPath = PathResolver::ExeDir();

        if (ImGui::Button("< ..")) {
            currentPath = currentPath.parent_path();
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            // dynamic view
        }
        ImGui::SameLine();
        if (ImGui::Button("Go to Assets")) {
            currentPath = fs::path(PathResolver::ExeDir()) / "assets";
            if (!fs::exists(currentPath)) {
                currentPath = PathResolver::ExeDir();
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::InputTextWithHint("##ContentSearch", "Filter...", m_contentSearch, IM_ARRAYSIZE(m_contentSearch));

        ImGui::SameLine();
        ImGui::TextDisabled("| Dir: %s", currentPath.filename().string().c_str());
        ImGui::Separator();

        try {
            for (auto& entry : fs::directory_iterator(currentPath)) {
                auto filename = entry.path().filename().string();
                if (m_contentSearch[0] != '\0') {
                    if (!ContainsCaseInsensitive(filename, m_contentSearch)) {
                        continue;
                    }
                }

                if (entry.is_directory()) {
                    if (ImGui::Selectable(("[Dir] " + filename).c_str())) {
                        currentPath = entry.path();
                    }
                } else {
                    ImGui::Selectable(filename.c_str());
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        std::string fullPathStr = entry.path().string();
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", fullPathStr.c_str(), fullPathStr.size() + 1);
                        ImGui::Text("Dragging %s", filename.c_str());
                        ImGui::EndDragDropSource();
                    }
                }
            }
        } catch (...) {
            ImGui::Text("(error reading directory)");
        }
        ImGui::End();
    }

    // === Console Panel ===
    if (m_showConsole) {
        ImGui::Begin("Console", &m_showConsole);

        static bool filterInfo = true;
        static bool filterWarn = true;
        static bool filterError = true;
        static bool autoScroll = true;
        static char searchBuffer[128] = "";

        if (ImGui::Button("Clear")) {
            s_consoleBuffer.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy All")) {
            std::string all;
            for (auto& entry : s_consoleBuffer) {
                all += entry.message + "\n";
            }
            ImGui::SetClipboardText(all.c_str());
        }

        // Filtros e busca
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.9f, 0.9f, 1.0f));
        ImGui::Checkbox("Info", &filterInfo); ImGui::SameLine();
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.45f, 0.05f, 1.0f));
        ImGui::Checkbox("Warns", &filterWarn); ImGui::SameLine();
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.95f, 1.0f));
        ImGui::Checkbox("Errors", &filterError); ImGui::SameLine();
        ImGui::PopStyleColor();

        ImGui::Checkbox("Auto-Scroll", &autoScroll);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputTextWithHint("##SearchLogs", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));

        ImGui::Separator();

        // Reservar espaço para a linha de comandos do terminal Lua no rodapé
        float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() + 5.0f;
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (auto& entry : s_consoleBuffer) {
            if (entry.level == LogLevel::Info && !filterInfo) continue;
            if (entry.level == LogLevel::Warn && !filterWarn) continue;
            if (entry.level == LogLevel::Error && !filterError) continue;

            if (searchBuffer[0] != '\0') {
                if (entry.message.find(searchBuffer) == std::string::npos) {
                    continue;
                }
            }

            ImVec4 color;
            switch (entry.level) {
                case LogLevel::Info:  color = ImVec4(0.0f, 0.9f, 0.9f, 1.0f); break;     // Cyber Cyan
                case LogLevel::Warn:  color = ImVec4(1.0f, 0.45f, 0.05f, 1.0f); break; // Sunset Orange
                case LogLevel::Error: color = ImVec4(1.0f, 0.2f, 0.95f, 1.0f); break;  // Hot Magenta
                default:              color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;
            }
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(entry.message.c_str());
            ImGui::PopStyleColor();
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::Separator();

        static char commandBuf[512] = "";
        ImGui::PushItemWidth(-1);
        if (ImGui::InputTextWithHint("##lua_cmd", "Enter Lua command (e.g. Engine.spawn('Sprite') or print('hello'))...", commandBuf, sizeof(commandBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::string cmd = commandBuf;
            if (!cmd.empty()) {
                Log::Info("> {}", cmd);
                auto* scripting = Engine::Get().GetSystem<ScriptSystem>();
                if (scripting) {
                    auto result = scripting->GetLua().safe_script(cmd, sol::script_pass_on_error);
                    if (!result.valid()) {
                        sol::error err = result;
                        Log::Error("Lua: {}", err.what());
                    }
                }
                commandBuf[0] = '\0';
                ImGui::SetKeyboardFocusHere(-1); // Mantém foco no input
            }
        }
        ImGui::PopItemWidth();

        ImGui::End();
    }

    // === Save Scene As dialog ===
    if (ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char pathBuf[512] = "scene.scene";
        ImGui::InputText("Path", pathBuf, sizeof(pathBuf));
        ImGui::Text("Save in assets/scenes/ relative to project");
        if (ImGui::Button("Save")) {
            auto sceneForSave = Engine::Get().GetSceneStack().Active();
            if (sceneForSave) {
                SceneSerializer::SaveToFile(*sceneForSave, pathBuf);
                m_sceneFilePath = pathBuf;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // === Load Scene dialog ===
    if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char pathBuf[512] = "scene.scene";
        ImGui::InputText("Path", pathBuf, sizeof(pathBuf));
        if (ImGui::Button("Load")) {
            auto sceneForLoad = Engine::Get().GetSceneStack().Active();
            if (sceneForLoad) {
                if (m_mode != EditorMode::Editing) {
                    OnStop();
                }
                SceneSerializer::LoadFromFile(*sceneForLoad, pathBuf);
                m_sceneFilePath = pathBuf;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End(); // DockSpace
    DrawUnifiedVisualCodingWorkspace();
    DrawCVarEditor();
    DrawProfilerWindow();

    // Restore default framebuffer for ImGui final render
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EditorSystem::ResetLayout() {
    m_layoutInitialized = false;
    m_showHierarchy = true;
    m_showInspector = true;
    m_showContentBrowser = true;
    m_showConsole = true;
    m_showShaderEditor = true;
    m_showBTEditor = true;
    m_showCVarEditor = true;
    m_showModernNodeEditor = true;
    Log::Info("EditorSystem: Layout reset requested (will rebuild on next frame).");
}

void EditorSystem::ConfigureGlobalIniPath() {
    namespace fs = std::filesystem;
    static std::string iniPathStorage;

    fs::path candidate = fs::path(PathResolver::ExeDir());
    fs::path sdkRoot;
    for (int depth = 0; depth < 6; ++depth) {
        if (fs::exists(candidate / "cmake" / "StarlightEngineConfig.cmake")) {
            sdkRoot = candidate;
            break;
        }
        if (fs::exists(candidate / "StarlightEngine" / "cmake" / "StarlightEngineConfig.cmake")) {
            sdkRoot = candidate / "StarlightEngine";
            break;
        }
        if (!candidate.has_parent_path()) break;
        candidate = candidate.parent_path();
    }

    if (sdkRoot.empty()) {
        sdkRoot = fs::path(PathResolver::ExeDir());
        Log::Warn("EditorSystem: Could not locate SDK root; using exe dir for imgui.ini.");
    }

    iniPathStorage = (sdkRoot / "imgui_editor_v2.ini").string();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = iniPathStorage.c_str();
    Log::Info("EditorSystem: imgui.ini path = {}", iniPathStorage);

    // Validate if the ini file actually contains docking settings to prevent clean/corrupted file locks
    bool validIniLoaded = false;
    if (fs::exists(iniPathStorage) && fs::file_size(iniPathStorage) > 100) {
        std::ifstream f(iniPathStorage);
        if (f.is_open()) {
            std::string line;
            while (std::getline(f, line)) {
                if (line.find("[Docking][Data]") != std::string::npos) {
                    validIniLoaded = true;
                    break;
                }
            }
        }
    }

    // Force reloading the newly defined INI file to override the default imgui.ini loaded during startup
    if (validIniLoaded) {
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
        m_layoutInitialized = true;
        Log::Info("EditorSystem: Successfully loaded valid docking layout from disk.");
    } else {
        // If file doesn't exist, has no docking data, or is empty, force a default layout build
        m_layoutInitialized = false;
        Log::Warn("EditorSystem: imgui_editor.ini is missing, empty, or lacks docking data. Forcing clean layout rebuild.");
        if (fs::exists(iniPathStorage)) {
            try {
                fs::remove(iniPathStorage);
            } catch (...) {}
        }
    }
}

void EditorSystem::BuildDefaultDockLayout(ImGuiID dockspaceID) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (viewport->Size.x <= 100.0f || viewport->Size.y <= 100.0f) {
        // Wait until viewport has a valid size (ImGui frame-zero safety check)
        return;
    }

    // Force show-flags for all main panels to guarantee they render and get docked properly
    m_showHierarchy = true;
    m_showInspector = true;
    m_showContentBrowser = true;
    m_showConsole = true;
    m_showUnifiedVisualCoding = true;

    ImGui::DockBuilderRemoveNode(dockspaceID);
    ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

    // 1) Split main area horizontally: left sidebar | rest
    ImGuiID leftSidebarId = 0;
    ImGuiID rightAreaId = 0;
    ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.22f, &leftSidebarId, &rightAreaId);

    // 2) Split the rest horizontally: viewport | right sidebar
    ImGuiID viewportDockId = 0;
    ImGuiID rightSidebarId = 0;
    ImGui::DockBuilderSplitNode(rightAreaId, ImGuiDir_Left, 0.70f, &viewportDockId, &rightSidebarId);

    // 3) Split left sidebar vertically: Hierarchy (top) | Inspector (bottom)
    ImGuiID hierarchyDockId = 0;
    ImGuiID inspectorDockId = 0;
    ImGui::DockBuilderSplitNode(leftSidebarId, ImGuiDir_Up, 0.55f, &hierarchyDockId, &inspectorDockId);

    // 4) Split right sidebar vertically: Console (top) | Content Browser (bottom)
    ImGuiID consoleDockId = 0;
    ImGuiID contentBrowserDockId = 0;
    ImGui::DockBuilderSplitNode(rightSidebarId, ImGuiDir_Up, 0.50f, &consoleDockId, &contentBrowserDockId);

    ImGui::DockBuilderDockWindow("Viewport",          viewportDockId);
    ImGui::DockBuilderDockWindow("Hierarchy",         hierarchyDockId);
    ImGui::DockBuilderDockWindow("Inspector",         inspectorDockId);
    ImGui::DockBuilderDockWindow("Console",           consoleDockId);
    ImGui::DockBuilderDockWindow("Content Browser",   contentBrowserDockId);
    ImGui::DockBuilderDockWindow("Unified Visual Scripting Editor", viewportDockId);

    ImGui::DockBuilderFinish(dockspaceID);
    m_layoutInitialized = true;
    Log::Info("EditorSystem: Default dock layout applied (Unreal-style: 4 regions).");

    // Force immediate save of the default layout to disk
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SaveIniSettingsToDisk(io.IniFilename);
    Log::Info("EditorSystem: Default layout settings written to disk immediately.");
}



static std::vector<ModernNode> s_shaderNodes;
static std::vector<ModernLink> s_shaderLinks;
static bool s_shaderNodesInitialized = false;
static int s_nextShaderId = 1;

static std::vector<ModernNode> s_btNodes;
static std::vector<ModernLink> s_btLinks;
static bool s_btNodesInitialized = false;
static int s_nextBtId = 1;



static std::vector<ModernNode> s_logicNodes;
static std::vector<ModernLink> s_logicLinks;
static bool s_logicInitialized = false;
static int s_nextLogicId = 1;

void EditorSystem::DrawUnifiedVisualCodingWorkspace() {
    Log::Info("TRACE: DrawUnifiedVisualCodingWorkspace entry. activeTab={}", m_activeVisualCodingTab);
    if (m_showShaderEditor) {
        m_showUnifiedVisualCoding = true;
        m_activeVisualCodingTab = 2;
        m_showShaderEditor = false;
    }
    if (m_showBTEditor) {
        m_showUnifiedVisualCoding = true;
        m_activeVisualCodingTab = 1;
        m_showBTEditor = false;
    }
    if (m_showModernNodeEditor) {
        m_showUnifiedVisualCoding = true;
        m_activeVisualCodingTab = 0;
        m_showModernNodeEditor = false;
    }

    if (!m_showUnifiedVisualCoding) {
        Log::Info("TRACE: DrawUnifiedVisualCodingWorkspace returning because m_showUnifiedVisualCoding is false");
        return;
    }

    Log::Info("TRACE: DrawUnifiedVisualCodingWorkspace - Begin window");
    ImGui::Begin("Unified Visual Scripting Editor", &m_showUnifiedVisualCoding);

    if (ImGui::BeginTabBar("VisualCodingTabs")) {
        ImGuiTabItemFlags tab0Flags = (m_activeVisualCodingTab == 0) ? ImGuiTabItemFlags_SetSelected : 0;
        ImGuiTabItemFlags tab1Flags = (m_activeVisualCodingTab == 1) ? ImGuiTabItemFlags_SetSelected : 0;
        ImGuiTabItemFlags tab2Flags = (m_activeVisualCodingTab == 2) ? ImGuiTabItemFlags_SetSelected : 0;

        if (ImGui::BeginTabItem("Game Logic (Snake)", nullptr, tab0Flags)) {
            m_activeVisualCodingTab = 0;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Behavior Tree (AI Decisions)", nullptr, tab1Flags)) {
            m_activeVisualCodingTab = 1;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Shader Editor (GPU Canvas)", nullptr, tab2Flags)) {
            m_activeVisualCodingTab = 2;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    Log::Info("TRACE: DrawUnifiedVisualCodingWorkspace - Dispatching tab. activeTab={}", m_activeVisualCodingTab);
    if (m_activeVisualCodingTab == 0) {
        DrawLogicEditorCanvas();
    } else if (m_activeVisualCodingTab == 1) {
        DrawBtEditorCanvas();
    } else if (m_activeVisualCodingTab == 2) {
        DrawShaderEditorCanvas();
    }

    Log::Info("TRACE: DrawUnifiedVisualCodingWorkspace - End window");
    ImGui::End();
}

void EditorSystem::DrawLogicEditorCanvas() {
    Log::Info("TRACE: DrawLogicEditorCanvas entry. s_logicInitialized={}", s_logicInitialized);

    if (!s_logicInitialized) {
        auto addNode = [](std::string name, std::string type, ImColor color, std::vector<std::string> inputs, std::vector<std::string> outputs, ImVec2 pos) {
            ModernNode node;
            node.id = s_nextLogicId++;
            node.name = name;
            node.type = type;
            node.color = color;
            for (const auto& inName : inputs) {
                node.inputPins.push_back({ s_nextLogicId++, inName });
            }
            for (const auto& outName : outputs) {
                node.outputPins.push_back({ s_nextLogicId++, outName });
            }
            s_logicNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), pos);
            return node;
        };

        auto addLink = [](int fromPin, int toPin) {
            ModernLink link;
            link.id = s_nextLogicId++;
            link.fromPin = fromPin;
            link.toPin = toPin;
            s_logicLinks.push_back(link);
        };

        ax::NodeEditor::SetCurrentEditor(m_logicEditorContext);

        auto n1 = addNode("Initialize Game", "Event", ImColor(255, 0, 149), {}, { "OnStart" }, ImVec2(100, 100));
        auto n2 = addNode("Setup Snake State", "Action", ImColor(0, 240, 255), { "Start" }, { "State Out" }, ImVec2(320, 100));
        auto n3 = addNode("On Frame Update", "Event", ImColor(255, 0, 149), {}, { "Tick (dt)" }, ImVec2(100, 320));
        auto n4 = addNode("Read Input Axes", "Action", ImColor(0, 240, 255), { "Tick" }, { "Axis X", "Axis Y" }, ImVec2(320, 260));
        auto n5 = addNode("Movement Timer", "Logic", ImColor(255, 217, 26), { "Delta Time", "Speed" }, { "Elapsed" }, ImVec2(320, 420));
        auto n6 = addNode("Move Head Pos", "Action", ImColor(140, 30, 255), { "Trigger", "Dir X", "Dir Y" }, { "New Pos" }, ImVec2(580, 360));
        auto n7 = addNode("Check Wall Bounds", "Logic", ImColor(255, 110, 0), { "Position" }, { "Out Bounds" }, ImVec2(820, 220));
        auto n8 = addNode("Check Self Collision", "Logic", ImColor(255, 110, 0), { "Position" }, { "Collided" }, ImVec2(820, 340));
        auto n9 = addNode("Check Food Eat", "Logic", ImColor(255, 110, 0), { "Position" }, { "Ate Food", "Food Type" }, ImVec2(820, 480));
        auto n10 = addNode("Game Over Handler", "Action", ImColor(255, 50, 50), { "Die Trigger" }, { "Play Death", "Shake Screen" }, ImVec2(1060, 260));
        auto n11 = addNode("Apply Eat Effects", "Action", ImColor(50, 220, 50), { "Ate Food", "Type" }, { "Play Coin", "Spawn Parts", "Add Score" }, ImVec2(1060, 460));
        auto n12 = addNode("Audio Player", "Action", ImColor(140, 30, 255), { "Coin Sfx", "Death Sfx" }, {}, ImVec2(1300, 340));
        auto n13 = addNode("Particle System", "Action", ImColor(0, 240, 255), { "Spawn FX" }, {}, ImVec2(1300, 480));
        auto n14 = addNode("Render UI Frame", "Rendering", ImColor(255, 0, 149), { "Draw Tick", "Score" }, {}, ImVec2(580, 560));

        addLink(n1.outputPins[0].id, n2.inputPins[0].id);
        addLink(n3.outputPins[0].id, n4.inputPins[0].id);
        addLink(n3.outputPins[0].id, n5.inputPins[0].id);
        addLink(n5.outputPins[0].id, n6.inputPins[0].id);
        addLink(n4.outputPins[0].id, n6.inputPins[1].id);
        addLink(n4.outputPins[1].id, n6.inputPins[2].id);
        addLink(n6.outputPins[0].id, n7.inputPins[0].id);
        addLink(n6.outputPins[0].id, n8.inputPins[0].id);
        addLink(n6.outputPins[0].id, n9.inputPins[0].id);
        addLink(n7.outputPins[0].id, n10.inputPins[0].id);
        addLink(n8.outputPins[0].id, n10.inputPins[0].id);
        addLink(n9.outputPins[0].id, n11.inputPins[0].id);
        addLink(n9.outputPins[1].id, n11.inputPins[1].id);
        addLink(n11.outputPins[0].id, n12.inputPins[0].id);
        addLink(n10.outputPins[0].id, n12.inputPins[1].id);
        addLink(n11.outputPins[1].id, n13.inputPins[0].id);
        addLink(n3.outputPins[0].id, n14.inputPins[0].id);
        addLink(n11.outputPins[2].id, n14.inputPins[1].id);

        ax::NodeEditor::SetCurrentEditor(nullptr);
        s_logicInitialized = true;
    }

    Log::Info("TRACE: DrawLogicEditorCanvas - Setting editor context to {}", (void*)m_logicEditorContext);
    ax::NodeEditor::SetCurrentEditor(m_logicEditorContext);
    Log::Info("TRACE: DrawLogicEditorCanvas - Calling ax::NodeEditor::Begin");
    ax::NodeEditor::Begin("Logic Editor Canvas");

    Log::Info("TRACE: DrawLogicEditorCanvas - Drawing {} nodes", s_logicNodes.size());
    for (const auto& node : s_logicNodes) {
        ax::NodeEditor::BeginNode(ax::NodeEditor::NodeId(node.id));
        
        ImGui::TextColored(node.color.Value, "%s", node.name.c_str());
        ImGui::TextDisabled("Type: %s", node.type.c_str());
        ImGui::Separator();
        
        ImGui::BeginGroup();
        for (const auto& pin : node.inputPins) {
            ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(pin.id), ax::NodeEditor::PinKind::Input);
            ImGui::Text("-> %s", pin.name.c_str());
            ax::NodeEditor::EndPin();
        }
        ImGui::EndGroup();
        
        ImGui::SameLine(180.0f);
        
        ImGui::BeginGroup();
        for (const auto& pin : node.outputPins) {
            ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(pin.id), ax::NodeEditor::PinKind::Output);
            ImGui::Text("%s ->", pin.name.c_str());
            ax::NodeEditor::EndPin();
        }
        ImGui::EndGroup();

        ax::NodeEditor::EndNode();
    }

    Log::Info("TRACE: DrawLogicEditorCanvas - Drawing {} links", s_logicLinks.size());
    for (const auto& link : s_logicLinks) {
        ax::NodeEditor::Link(ax::NodeEditor::LinkId(link.id), ax::NodeEditor::PinId(link.fromPin), ax::NodeEditor::PinId(link.toPin));
    }

    Log::Info("TRACE: DrawLogicEditorCanvas - Calling ax::NodeEditor::End");
    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
    Log::Info("TRACE: DrawLogicEditorCanvas - Exit");
}

void EditorSystem::DrawBtEditorCanvas() {
    if (!s_btNodesInitialized) {
        auto addNode = [](std::string name, std::string type, ImColor color, std::vector<std::string> inputs, std::vector<std::string> outputs, ImVec2 pos) {
            ModernNode node;
            node.id = s_nextBtId++;
            node.name = name;
            node.type = type;
            node.color = color;
            for (const auto& inName : inputs) {
                node.inputPins.push_back({ s_nextBtId++, inName });
            }
            for (const auto& outName : outputs) {
                node.outputPins.push_back({ s_nextBtId++, outName });
            }
            s_btNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), pos);
            return node;
        };

        auto addLink = [](int fromPin, int toPin) {
            ModernLink link;
            link.id = s_nextBtId++;
            link.fromPin = fromPin;
            link.toPin = toPin;
            s_btLinks.push_back(link);
        };

        ax::NodeEditor::SetCurrentEditor(m_btEditorContext);

        auto n1 = addNode("SnakeRoot (Selector)", "Selector", ImColor(255, 110, 0), {}, { "Sequence List" }, ImVec2(50, 300));
        auto n2 = addNode("GameLoop (Sequence)", "Sequence", ImColor(0, 240, 255), { "Parent" }, { "Actions" }, ImVec2(250, 300));
        
        auto n3 = addNode("CheckInput (Sequence)", "Sequence", ImColor(0, 240, 255), { "Parent" }, { "Action" }, ImVec2(450, 100));
        auto n4 = addNode("AnyKeyPressed?", "Condition", ImColor(255, 0, 149), { "Check" }, { "Yes" }, ImVec2(650, 60));
        auto n5 = addNode("UpdateNextDir", "Action", ImColor(140, 30, 255), { "Trigger" }, {}, ImVec2(850, 60));

        auto n6 = addNode("CheckTimer (Sequence)", "Sequence", ImColor(0, 240, 255), { "Parent" }, { "Action" }, ImVec2(450, 230));
        auto n7 = addNode("IsTimeStepElapsed?", "Condition", ImColor(255, 0, 149), { "Check" }, { "Yes" }, ImVec2(650, 190));
        auto n8 = addNode("MoveSnakeHead", "Action", ImColor(140, 30, 255), { "Trigger" }, {}, ImVec2(850, 190));

        auto n9 = addNode("CheckCollision (Sequence)", "Sequence", ImColor(0, 240, 255), { "Parent" }, { "Action" }, ImVec2(450, 360));
        auto n10 = addNode("HitWallOrSelf?", "Condition", ImColor(255, 0, 149), { "Check" }, { "Yes" }, ImVec2(650, 320));
        auto n11 = addNode("TriggerGameOver", "Action", ImColor(255, 50, 50), { "Trigger" }, {}, ImVec2(850, 320));

        auto n12 = addNode("CheckFood (Sequence)", "Sequence", ImColor(0, 240, 255), { "Parent" }, { "Action" }, ImVec2(450, 490));
        auto n13 = addNode("IsHeadOnFood?", "Condition", ImColor(255, 0, 149), { "Check" }, { "Yes" }, ImVec2(650, 450));
        auto n14 = addNode("EatAndGrow", "Action", ImColor(50, 220, 50), { "Trigger" }, {}, ImVec2(850, 450));

        auto n15 = addNode("RenderGameFrame", "Action", ImColor(255, 217, 26), { "Trigger" }, {}, ImVec2(450, 620));

        addLink(n1.outputPins[0].id, n2.inputPins[0].id);
        
        addLink(n2.outputPins[0].id, n3.inputPins[0].id);
        addLink(n3.outputPins[0].id, n4.inputPins[0].id);
        addLink(n4.outputPins[0].id, n5.inputPins[0].id);

        addLink(n2.outputPins[0].id, n6.inputPins[0].id);
        addLink(n6.outputPins[0].id, n7.inputPins[0].id);
        addLink(n7.outputPins[0].id, n8.inputPins[0].id);

        addLink(n2.outputPins[0].id, n9.inputPins[0].id);
        addLink(n9.outputPins[0].id, n10.inputPins[0].id);
        addLink(n10.outputPins[0].id, n11.inputPins[0].id);

        addLink(n2.outputPins[0].id, n12.inputPins[0].id);
        addLink(n12.outputPins[0].id, n13.inputPins[0].id);
        addLink(n13.outputPins[0].id, n14.inputPins[0].id);

        addLink(n2.outputPins[0].id, n15.inputPins[0].id);

        ax::NodeEditor::SetCurrentEditor(nullptr);
        s_btNodesInitialized = true;
    }

    if (ImGui::Button("Export to Lua Behavior Tree", ImVec2(220, 30))) {
        std::string luaCode = "-- Generated Behavior Tree from Visual Graph Editor\n";
        luaCode += "local BehaviorTree = \n";
        luaCode += ExportNodeLua(1, 0, s_btNodes, s_btLinks);
        luaCode += "\nreturn BehaviorTree\n";
        
        std::string fullPath = PathResolver::ExeDir() + "/assets/scripts/custom_behavior.lua";
        std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());
        std::ofstream out(fullPath);
        if (out.is_open()) {
            out << luaCode;
            out.close();
            Log::Info("[Behavior Tree] Exported tree successfully to: " + fullPath);
        } else {
            Log::Error("[Behavior Tree] Failed to save exported Lua script: " + fullPath);
        }
    }
    
    ImGui::SameLine();
    ImGui::TextDisabled("RMB Canvas: Add Node | RMB Link/Node: Delete");

    ax::NodeEditor::SetCurrentEditor(m_btEditorContext);
    ax::NodeEditor::Begin("Behavior Tree Editor Canvas");

    for (const auto& node : s_btNodes) {
        bool isNodeActive = IsBtNodeActive(node.name);

        if (isNodeActive) {
            float pulse = 1.0f + 0.25f * std::sin((float)SDL_GetTicks() * 0.008f);
            ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(0, 255, 230, 255));
            ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeBorderWidth, 3.0f * pulse);
        }

        ax::NodeEditor::BeginNode(ax::NodeEditor::NodeId(node.id));
        
        ImGui::TextColored(node.color.Value, "%s", node.name.c_str());
        ImGui::TextDisabled("Type: %s", node.type.c_str());
        ImGui::Separator();
        
        ImGui::BeginGroup();
        for (const auto& pin : node.inputPins) {
            ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(pin.id), ax::NodeEditor::PinKind::Input);
            ImGui::Text("-> %s", pin.name.c_str());
            ax::NodeEditor::EndPin();
        }
        ImGui::EndGroup();
        
        ImGui::SameLine(180.0f);
        
        ImGui::BeginGroup();
        for (const auto& pin : node.outputPins) {
            ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(pin.id), ax::NodeEditor::PinKind::Output);
            ImGui::Text("%s ->", pin.name.c_str());
            ax::NodeEditor::EndPin();
        }
        ImGui::EndGroup();

        ax::NodeEditor::EndNode();

        if (isNodeActive) {
            ax::NodeEditor::PopStyleVar();
            ax::NodeEditor::PopStyleColor();
        }
    }

    for (const auto& link : s_btLinks) {
        ax::NodeEditor::Link(ax::NodeEditor::LinkId(link.id), ax::NodeEditor::PinId(link.fromPin), ax::NodeEditor::PinId(link.toPin));
    }

    if (ax::NodeEditor::BeginCreate()) {
        ax::NodeEditor::PinId startPinId, endPinId;
        if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId)) {
            if (ax::NodeEditor::AcceptNewItem()) {
                ModernLink newLink;
                newLink.id = s_nextBtId++;
                newLink.fromPin = (int)startPinId.Get();
                newLink.toPin = (int)endPinId.Get();
                s_btLinks.push_back(newLink);
            }
        }
    }
    ax::NodeEditor::EndCreate();

    if (ax::NodeEditor::BeginDelete()) {
        ax::NodeEditor::NodeId deletedNodeId;
        while (ax::NodeEditor::QueryDeletedNode(&deletedNodeId)) {
            if (ax::NodeEditor::AcceptDeletedItem()) {
                s_btNodes.erase(
                    std::remove_if(s_btNodes.begin(), s_btNodes.end(),
                        [&](const ModernNode& n) { return n.id == (int)deletedNodeId.Get(); }),
                    s_btNodes.end()
                );
            }
        }
        ax::NodeEditor::LinkId deletedLinkId;
        while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {
            if (ax::NodeEditor::AcceptDeletedItem()) {
                s_btLinks.erase(
                    std::remove_if(s_btLinks.begin(), s_btLinks.end(),
                        [&](const ModernLink& l) { return l.id == (int)deletedLinkId.Get(); }),
                    s_btLinks.end()
                );
            }
        }
    }
    ax::NodeEditor::EndDelete();

    ax::NodeEditor::Suspend();
    if (ax::NodeEditor::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("AddBtNodePopup");
    }

    if (ImGui::BeginPopup("AddBtNodePopup")) {
        ImVec2 clickPos = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos());
        if (ImGui::MenuItem("Selector Node")) {
            ModernNode node;
            node.id = s_nextBtId++;
            node.name = "Selector Node";
            node.type = "Selector";
            node.color = ImColor(255, 110, 0);
            node.outputPins.push_back({ s_nextBtId++, "Sequence List" });
            s_btNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        if (ImGui::MenuItem("Sequence Node")) {
            ModernNode node;
            node.id = s_nextBtId++;
            node.name = "Sequence Node";
            node.type = "Sequence";
            node.color = ImColor(0, 240, 255);
            node.inputPins.push_back({ s_nextBtId++, "Parent" });
            node.outputPins.push_back({ s_nextBtId++, "Actions" });
            s_btNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        if (ImGui::MenuItem("Action Node")) {
            ModernNode node;
            node.id = s_nextBtId++;
            node.name = "Action Node";
            node.type = "Action";
            node.color = ImColor(140, 30, 255);
            node.inputPins.push_back({ s_nextBtId++, "Trigger" });
            s_btNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        if (ImGui::MenuItem("Condition Node")) {
            ModernNode node;
            node.id = s_nextBtId++;
            node.name = "Condition Node";
            node.type = "Condition";
            node.color = ImColor(255, 0, 149);
            node.inputPins.push_back({ s_nextBtId++, "Check" });
            node.outputPins.push_back({ s_nextBtId++, "Yes" });
            s_btNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        ImGui::EndPopup();
    }
    ax::NodeEditor::Resume();

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}

void EditorSystem::DrawShaderEditorCanvas() {
    if (!s_shaderNodesInitialized) {
        auto addNode = [](std::string name, std::string type, ImColor color, std::vector<std::string> inputs, std::vector<std::string> outputs, ImVec2 pos) {
            ModernNode node;
            node.id = s_nextShaderId++;
            node.name = name;
            node.type = type;
            node.color = color;
            for (const auto& inName : inputs) {
                node.inputPins.push_back({ s_nextShaderId++, inName });
            }
            for (const auto& outName : outputs) {
                node.outputPins.push_back({ s_nextShaderId++, outName });
            }
            s_shaderNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), pos);
            return node;
        };

        auto addLink = [](int fromPin, int toPin) {
            ModernLink link;
            link.id = s_nextShaderId++;
            link.fromPin = fromPin;
            link.toPin = toPin;
            s_shaderLinks.push_back(link);
        };

        ax::NodeEditor::SetCurrentEditor(m_shaderEditorContext);

        auto n1 = addNode("Texture Sample", "Texture", ImColor(0, 240, 255), {}, { "Color" }, ImVec2(50, 100));
        auto n2 = addNode("Multiplier", "MathMul", ImColor(140, 30, 255), { "A", "B" }, { "Result" }, ImVec2(300, 150));
        auto n3 = addNode("Neon Tint", "Color", ImColor(255, 0, 149), {}, { "RGB" }, ImVec2(50, 250));
        auto n4 = addNode("FragColor", "Output", ImColor(255, 110, 0), { "Final Color" }, {}, ImVec2(550, 180));

        addLink(n1.outputPins[0].id, n2.inputPins[0].id);
        addLink(n3.outputPins[0].id, n2.inputPins[1].id);
        addLink(n2.outputPins[0].id, n4.inputPins[0].id);

        ax::NodeEditor::SetCurrentEditor(nullptr);
        s_shaderNodesInitialized = true;
    }

    if (ImGui::Button("Compile to GLSL Shader", ImVec2(200, 30))) {
        std::string fragColorExpr = ResolveShaderExpression(4, s_shaderNodes, s_shaderLinks);
        std::string glslCode = 
            "#version 450 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoord;\n"
            "uniform sampler2D uTexture;\n\n"
            "void main() {\n"
            "    FragColor = " + fragColorExpr + ";\n"
            "}\n";
        
        std::string fullPath = PathResolver::ExeDir() + "/assets/shaders/custom_node_shader.frag";
        std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());
        std::ofstream out(fullPath);
        if (out.is_open()) {
            out << glslCode;
            out.close();
            Log::Info("[Shader Compiler] Compiled shader dynamically from Visual Graph. Saved to: " + fullPath);
        } else {
            Log::Error("[Shader Compiler] Failed to save generated shader file: " + fullPath);
        }
    }
    
    ImGui::SameLine();
    ImGui::TextDisabled("RMB Canvas: Add Node | RMB Link/Node: Delete");

    ax::NodeEditor::SetCurrentEditor(m_shaderEditorContext);
    ax::NodeEditor::Begin("Shader Editor Canvas");

    for (const auto& node : s_shaderNodes) {
        ax::NodeEditor::BeginNode(ax::NodeEditor::NodeId(node.id));
        
        ImGui::TextColored(node.color.Value, "%s", node.name.c_str());
        ImGui::TextDisabled("Type: %s", node.type.c_str());
        ImGui::Separator();
        
        ImGui::BeginGroup();
        for (const auto& pin : node.inputPins) {
            ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(pin.id), ax::NodeEditor::PinKind::Input);
            ImGui::Text("-> %s", pin.name.c_str());
            ax::NodeEditor::EndPin();
        }
        ImGui::EndGroup();
        
        ImGui::SameLine(180.0f);
        
        ImGui::BeginGroup();
        for (const auto& pin : node.outputPins) {
            ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(pin.id), ax::NodeEditor::PinKind::Output);
            ImGui::Text("%s ->", pin.name.c_str());
            ax::NodeEditor::EndPin();
        }
        ImGui::EndGroup();

        ax::NodeEditor::EndNode();
    }

    for (const auto& link : s_shaderLinks) {
        ax::NodeEditor::Link(ax::NodeEditor::LinkId(link.id), ax::NodeEditor::PinId(link.fromPin), ax::NodeEditor::PinId(link.toPin));
    }

    if (ax::NodeEditor::BeginCreate()) {
        ax::NodeEditor::PinId startPinId, endPinId;
        if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId)) {
            if (ax::NodeEditor::AcceptNewItem()) {
                ModernLink newLink;
                newLink.id = s_nextShaderId++;
                newLink.fromPin = (int)startPinId.Get();
                newLink.toPin = (int)endPinId.Get();
                s_shaderLinks.push_back(newLink);
            }
        }
    }
    ax::NodeEditor::EndCreate();

    if (ax::NodeEditor::BeginDelete()) {
        ax::NodeEditor::NodeId deletedNodeId;
        while (ax::NodeEditor::QueryDeletedNode(&deletedNodeId)) {
            if (ax::NodeEditor::AcceptDeletedItem()) {
                s_shaderNodes.erase(
                    std::remove_if(s_shaderNodes.begin(), s_shaderNodes.end(),
                        [&](const ModernNode& n) { return n.id == (int)deletedNodeId.Get(); }),
                    s_shaderNodes.end()
                );
            }
        }
        ax::NodeEditor::LinkId deletedLinkId;
        while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {
            if (ax::NodeEditor::AcceptDeletedItem()) {
                s_shaderLinks.erase(
                    std::remove_if(s_shaderLinks.begin(), s_shaderLinks.end(),
                        [&](const ModernLink& l) { return l.id == (int)deletedLinkId.Get(); }),
                    s_shaderLinks.end()
                );
            }
        }
    }
    ax::NodeEditor::EndDelete();

    ax::NodeEditor::Suspend();
    if (ax::NodeEditor::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("AddShaderNodePopup");
    }

    if (ImGui::BeginPopup("AddShaderNodePopup")) {
        ImVec2 clickPos = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos());
        if (ImGui::MenuItem("Texture Sample")) {
            ModernNode node;
            node.id = s_nextShaderId++;
            node.name = "Texture Sample";
            node.type = "Texture";
            node.color = ImColor(0, 240, 255);
            node.outputPins.push_back({ s_nextShaderId++, "Color" });
            s_shaderNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        if (ImGui::MenuItem("Multiplier")) {
            ModernNode node;
            node.id = s_nextShaderId++;
            node.name = "Multiplier";
            node.type = "MathMul";
            node.color = ImColor(140, 30, 255);
            node.inputPins.push_back({ s_nextShaderId++, "A" });
            node.inputPins.push_back({ s_nextShaderId++, "B" });
            node.outputPins.push_back({ s_nextShaderId++, "Result" });
            s_shaderNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        if (ImGui::MenuItem("Neon Tint")) {
            ModernNode node;
            node.id = s_nextShaderId++;
            node.name = "Neon Tint";
            node.type = "Color";
            node.color = ImColor(255, 0, 149);
            node.outputPins.push_back({ s_nextShaderId++, "RGB" });
            s_shaderNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        if (ImGui::MenuItem("FragColor")) {
            ModernNode node;
            node.id = s_nextShaderId++;
            node.name = "FragColor";
            node.type = "Output";
            node.color = ImColor(255, 110, 0);
            node.inputPins.push_back({ s_nextShaderId++, "Final Color" });
            s_shaderNodes.push_back(node);
            ax::NodeEditor::SetNodePosition(ax::NodeEditor::NodeId(node.id), clickPos);
        }
        ImGui::EndPopup();
    }
    ax::NodeEditor::Resume();

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}

void EditorSystem::DrawCVarEditor() {
    if (!m_showCVarEditor) return;

    ImGui::Begin("Developer Settings & CVars", &m_showCVarEditor);

    auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
    if (!cvarSys) {
        ImGui::Text("CVarSystem is not available.");
        ImGui::End();
        return;
    }

    static char searchBuf[128] = "";
    ImGui::InputText("Search CVars", searchBuf, sizeof(searchBuf));
    ImGui::Separator();

    auto cvars = cvarSys->GetCVars();
    
    ImGui::BeginChild("CVarListRegion");
    for (auto& cvar : cvars) {
        if (searchBuf[0] != '\0' && !ContainsCaseInsensitive(cvar.name, searchBuf) && !ContainsCaseInsensitive(cvar.description, searchBuf)) {
            continue;
        }

        ImGui::PushID(cvar.name.c_str());

        // Display CVar depending on its type
        if (cvar.type == CVarType::Bool) {
            bool val = std::get<bool>(cvar.value);
            if (ImGui::Checkbox(cvar.name.c_str(), &val)) {
                cvarSys->SetBool(cvar.name, val);
            }
        } else if (cvar.type == CVarType::Int) {
            int val = std::get<int>(cvar.value);
            if (cvar.name == "r_showProfiler" || cvar.name == "g_graphicsPreset" || cvar.name == "g_aiDifficulty") {
                int maxVal = (cvar.name == "g_aiDifficulty") ? 3 : 2;
                if (ImGui::SliderInt(cvar.name.c_str(), &val, 0, maxVal, "%d")) {
                    cvarSys->SetInt(cvar.name, val);
                }
            } else {
                if (ImGui::DragInt(cvar.name.c_str(), &val, 1.0f)) {
                    cvarSys->SetInt(cvar.name, val);
                }
            }
        } else if (cvar.type == CVarType::Float) {
            float val = std::get<float>(cvar.value);
            if (cvar.name.rfind("snd_", 0) == 0 || cvar.name == "r_exposure" || cvar.name == "r_fsr_sharpness") {
                if (ImGui::SliderFloat(cvar.name.c_str(), &val, 0.0f, 1.0f, "%.2f")) {
                    cvarSys->SetFloat(cvar.name, val);
                }
            } else if (cvar.name == "r_gamma") {
                if (ImGui::SliderFloat(cvar.name.c_str(), &val, 0.5f, 3.5f, "%.2f")) {
                    cvarSys->SetFloat(cvar.name, val);
                }
            } else {
                if (ImGui::DragFloat(cvar.name.c_str(), &val, 0.01f)) {
                    cvarSys->SetFloat(cvar.name, val);
                }
            }
        } else if (cvar.type == CVarType::String) {
            std::string val = std::get<std::string>(cvar.value);
            char buf[256];
            strncpy(buf, val.c_str(), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputText(cvar.name.c_str(), buf, sizeof(buf))) {
                cvarSys->SetString(cvar.name, buf);
            }
        }

        ImGui::SameLine();
        ImGui::TextDisabled("[?]");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", cvar.description.c_str());
        }

        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::End();
}

static std::vector<std::string> s_activeBtNodesThisFrame;

void EditorSystem::ReportActiveBtNode(const std::string& name) {
    if (std::find(s_activeBtNodesThisFrame.begin(), s_activeBtNodesThisFrame.end(), name) == s_activeBtNodesThisFrame.end()) {
        s_activeBtNodesThisFrame.push_back(name);
    }
}

void EditorSystem::ClearActiveBtNodes() {
    s_activeBtNodesThisFrame.clear();
}

bool EditorSystem::IsBtNodeActive(const std::string& name) const {
    for (const auto& activeName : s_activeBtNodesThisFrame) {
        std::string cleanNodeName = name;
        if (!cleanNodeName.empty() && cleanNodeName.back() == '?') {
            cleanNodeName.pop_back();
        }
        size_t idx = cleanNodeName.find(" (");
        if (idx != std::string::npos) {
            cleanNodeName = cleanNodeName.substr(0, idx);
        }
        
        // Case-insensitive comparison
        if (cleanNodeName.size() == activeName.size()) {
            bool match = true;
            for (size_t i = 0; i < cleanNodeName.size(); ++i) {
                if (std::tolower(cleanNodeName[i]) != std::tolower(activeName[i])) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
    }
    return false;
}

void EditorSystem::DrawProfilerWindow() {
    if (!m_showProfilerWindow) return;

    // Window colors styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.02f, 0.08f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.6f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));

    ImGui::SetNextWindowSize(ImVec2(400, 480), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("System Profiler", &m_showProfilerWindow)) {
        const auto& prof = Engine::Get().GetProfilerStats();
        
        // Sum total measured latency
        float totalTime = prof.renderTime + prof.scriptTime + prof.physicsTime + prof.audioTime + prof.updateTime;
        float realFrameTime = Engine::Get().GetTime().avgFrameTime * 1000.0f;
        if (realFrameTime <= 0.0f) realFrameTime = totalTime;

        // Record history
        m_frameTimeHistory[m_frameTimeHistoryIdx] = realFrameTime;
        m_frameTimeHistoryIdx = (m_frameTimeHistoryIdx + 1) % 120;

        // Update bounds
        if (prof.renderTime > 0.0f) {
            m_minRender = std::min(m_minRender, prof.renderTime);
            m_maxRender = std::max(m_maxRender, prof.renderTime);
        }
        if (prof.scriptTime > 0.0f) {
            m_minScript = std::min(m_minScript, prof.scriptTime);
            m_maxScript = std::max(m_maxScript, prof.scriptTime);
        }
        if (prof.physicsTime > 0.0f) {
            m_minPhysics = std::min(m_minPhysics, prof.physicsTime);
            m_maxPhysics = std::max(m_maxPhysics, prof.physicsTime);
        }
        if (prof.audioTime > 0.0f) {
            m_minAudio = std::min(m_minAudio, prof.audioTime);
            m_maxAudio = std::max(m_maxAudio, prof.audioTime);
        }
        if (prof.updateTime > 0.0f) {
            m_minUpdate = std::min(m_minUpdate, prof.updateTime);
            m_maxUpdate = std::max(m_maxUpdate, prof.updateTime);
        }
        if (totalTime > 0.0f) {
            m_minTotal = std::min(m_minTotal, totalTime);
            m_maxTotal = std::max(m_maxTotal, totalTime);
        }

        // Header telemetry info
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "STARLIGHT ADVANCED TELEMETRY");
        ImGui::Separator();
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", Engine::Get().GetTime().fps, realFrameTime);
        ImGui::Spacing();

        // 1. Frametime History Graph
        ImGui::Text("Frame Time History (Last 120 frames)");
        
        float avgFrame = 0.0f;
        for (int i = 0; i < 120; ++i) avgFrame += m_frameTimeHistory[i];
        avgFrame /= 120.0f;

        char graphLabel[64];
        snprintf(graphLabel, sizeof(graphLabel), "Avg: %.2f ms", avgFrame);
        
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.0f, 0.5f, 1.0f)); // Hot Magenta
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.08f, 0.04f, 0.12f, 0.50f));
        ImGui::PlotLines("##FrameTimePlot", m_frameTimeHistory, 120, m_frameTimeHistoryIdx, graphLabel, 0.0f, 33.3f, ImVec2(0, 80));
        ImGui::PopStyleColor(2);

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "CPU SUBSYSTEM PROGRESS (Target: 16.67 ms)");
        ImGui::Separator();

        // 2. Latency Bars with Cyberpunk coloring
        auto drawProfilerBar = [](const char* label, float timeMs, const ImVec4& color) {
            ImGui::Text("%-8s: %6.2f ms", label, timeMs);
            ImGui::SameLine();
            float fraction = timeMs / 16.67f;
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.08f, 0.04f, 0.12f, 0.50f));
            ImGui::ProgressBar(fraction, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 0.8f), "");
            ImGui::PopStyleColor(2);
        };

        drawProfilerBar("Render", prof.renderTime, ImVec4(0.0f, 1.0f, 0.9f, 1.0f));     // Cyber Cyan
        drawProfilerBar("Scripts", prof.scriptTime, ImVec4(1.0f, 0.0f, 0.5f, 1.0f));    // Hot Magenta
        drawProfilerBar("Physics", prof.physicsTime, ImVec4(1.0f, 0.4f, 0.0f, 1.0f));   // Sunset Orange
        drawProfilerBar("Audio", prof.audioTime, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));       // Gold
        drawProfilerBar("Update", prof.updateTime, ImVec4(0.7f, 0.0f, 1.0f, 1.0f));      // Neon Violet

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "DETAILED METRICS TABLE");
        ImGui::Separator();

        // 3. Detailed metrics table
        if (ImGui::BeginTable("MetricsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Subsystem", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Current (ms)");
            ImGui::TableSetupColumn("Min (ms)");
            ImGui::TableSetupColumn("Max (ms)");
            ImGui::TableSetupColumn("Budget %");
            ImGui::TableHeadersRow();

            auto addTableRow = [](const char* name, float current, float min, float max) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", name);
                
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", current);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", (min > 9000.0f) ? 0.0f : min);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", max);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.1f%%", (current / 16.67f) * 100.0f);
            };

            addTableRow("Render", prof.renderTime, m_minRender, m_maxRender);
            addTableRow("Scripts", prof.scriptTime, m_minScript, m_maxScript);
            addTableRow("Physics", prof.physicsTime, m_minPhysics, m_maxPhysics);
            addTableRow("Audio", prof.audioTime, m_minAudio, m_maxAudio);
            addTableRow("Update", prof.updateTime, m_minUpdate, m_maxUpdate);
            addTableRow("Total CPU", totalTime, m_minTotal, m_maxTotal);

            ImGui::EndTable();
        }

        ImGui::Spacing();
        if (ImGui::Button("Reset Min/Max Stats")) {
            m_minRender = m_minScript = m_minPhysics = m_minAudio = m_minUpdate = m_minTotal = 9999.0f;
            m_maxRender = m_maxScript = m_maxPhysics = m_maxAudio = m_maxUpdate = m_maxTotal = 0.0f;
        }
    }
    ImGui::End();

    ImGui::PopStyleColor(3);
}

static Rml::Input::KeyIdentifier TranslateKey(SDL_Keycode sdlkey) {
    switch (sdlkey) {
        case SDLK_UNKNOWN: return Rml::Input::KI_UNKNOWN;
        case SDLK_ESCAPE: return Rml::Input::KI_ESCAPE;
        case SDLK_SPACE: return Rml::Input::KI_SPACE;
        case SDLK_RETURN: return Rml::Input::KI_RETURN;
        case SDLK_KP_ENTER: return Rml::Input::KI_NUMPADENTER;
        case SDLK_BACKSPACE: return Rml::Input::KI_BACK;
        case SDLK_TAB: return Rml::Input::KI_TAB;
        case SDLK_LEFT: return Rml::Input::KI_LEFT;
        case SDLK_RIGHT: return Rml::Input::KI_RIGHT;
        case SDLK_UP: return Rml::Input::KI_UP;
        case SDLK_DOWN: return Rml::Input::KI_DOWN;
        case SDLK_0: return Rml::Input::KI_0;
        case SDLK_1: return Rml::Input::KI_1;
        case SDLK_2: return Rml::Input::KI_2;
        case SDLK_3: return Rml::Input::KI_3;
        case SDLK_4: return Rml::Input::KI_4;
        case SDLK_5: return Rml::Input::KI_5;
        case SDLK_6: return Rml::Input::KI_6;
        case SDLK_7: return Rml::Input::KI_7;
        case SDLK_8: return Rml::Input::KI_8;
        case SDLK_9: return Rml::Input::KI_9;
        case SDLK_a: return Rml::Input::KI_A;
        case SDLK_b: return Rml::Input::KI_B;
        case SDLK_c: return Rml::Input::KI_C;
        case SDLK_d: return Rml::Input::KI_D;
        case SDLK_e: return Rml::Input::KI_E;
        case SDLK_f: return Rml::Input::KI_F;
        case SDLK_g: return Rml::Input::KI_G;
        case SDLK_h: return Rml::Input::KI_H;
        case SDLK_i: return Rml::Input::KI_I;
        case SDLK_j: return Rml::Input::KI_J;
        case SDLK_k: return Rml::Input::KI_K;
        case SDLK_l: return Rml::Input::KI_L;
        case SDLK_m: return Rml::Input::KI_M;
        case SDLK_n: return Rml::Input::KI_N;
        case SDLK_o: return Rml::Input::KI_O;
        case SDLK_p: return Rml::Input::KI_P;
        case SDLK_q: return Rml::Input::KI_Q;
        case SDLK_r: return Rml::Input::KI_R;
        case SDLK_s: return Rml::Input::KI_S;
        case SDLK_t: return Rml::Input::KI_T;
        case SDLK_u: return Rml::Input::KI_U;
        case SDLK_v: return Rml::Input::KI_V;
        case SDLK_w: return Rml::Input::KI_W;
        case SDLK_x: return Rml::Input::KI_X;
        case SDLK_y: return Rml::Input::KI_Y;
        case SDLK_z: return Rml::Input::KI_Z;
        case SDLK_F1: return Rml::Input::KI_F1;
        case SDLK_F2: return Rml::Input::KI_F2;
        case SDLK_F3: return Rml::Input::KI_F3;
        case SDLK_F4: return Rml::Input::KI_F4;
        case SDLK_F5: return Rml::Input::KI_F5;
        case SDLK_F6: return Rml::Input::KI_F6;
        case SDLK_F7: return Rml::Input::KI_F7;
        case SDLK_F8: return Rml::Input::KI_F8;
        case SDLK_F9: return Rml::Input::KI_F9;
        case SDLK_F10: return Rml::Input::KI_F10;
        case SDLK_F11: return Rml::Input::KI_F11;
        case SDLK_F12: return Rml::Input::KI_F12;
        case SDLK_LSHIFT: return Rml::Input::KI_LSHIFT;
        case SDLK_RSHIFT: return Rml::Input::KI_RSHIFT;
        case SDLK_LCTRL: return Rml::Input::KI_LCONTROL;
        case SDLK_RCTRL: return Rml::Input::KI_RCONTROL;
        case SDLK_LALT: return Rml::Input::KI_LMENU;
        case SDLK_RALT: return Rml::Input::KI_RMENU;
        default: return Rml::Input::KI_UNKNOWN;
    }
}

void EditorSystem::ProcessEvent(const union SDL_Event* event) {
    if (m_mode != EditorMode::Playing || !s_rmlContext) return;

    int key_modifier_state = 0;
    SDL_Keymod mod = SDL_GetModState();
    if (mod & KMOD_SHIFT) key_modifier_state |= Rml::Input::KM_SHIFT;
    if (mod & KMOD_CTRL)  key_modifier_state |= Rml::Input::KM_CTRL;
    if (mod & KMOD_ALT)   key_modifier_state |= Rml::Input::KM_ALT;

    switch (event->type) {
        case SDL_MOUSEMOTION: {
            float localX = (float)event->motion.x - m_viewportScreenPosX;
            float localY = (float)event->motion.y - m_viewportScreenPosY;
            s_rmlContext->ProcessMouseMove((int)localX, (int)localY, key_modifier_state);
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            int button_index = -1;
            if (event->button.button == SDL_BUTTON_LEFT) button_index = 0;
            else if (event->button.button == SDL_BUTTON_RIGHT) button_index = 1;
            else if (event->button.button == SDL_BUTTON_MIDDLE) button_index = 2;

            if (button_index != -1) {
                if (event->type == SDL_MOUSEBUTTONDOWN) {
                    s_rmlContext->ProcessMouseButtonDown(button_index, key_modifier_state);
                } else {
                    s_rmlContext->ProcessMouseButtonUp(button_index, key_modifier_state);
                }
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            s_rmlContext->ProcessMouseWheel((float)-event->wheel.y, key_modifier_state);
            break;
        }
        case SDL_KEYDOWN: {
            Rml::Input::KeyIdentifier key = TranslateKey(event->key.keysym.sym);
            if (key != Rml::Input::KI_UNKNOWN) {
                s_rmlContext->ProcessKeyDown(key, key_modifier_state);
            }
            break;
        }
        case SDL_KEYUP: {
            Rml::Input::KeyIdentifier key = TranslateKey(event->key.keysym.sym);
            if (key != Rml::Input::KI_UNKNOWN) {
                s_rmlContext->ProcessKeyUp(key, key_modifier_state);
            }
            break;
        }
        case SDL_TEXTINPUT: {
            s_rmlContext->ProcessTextInput(event->text.text);
            break;
        }
    }
}

// Legacy DrawModernNodeEditor removed (unified inside DrawLogicEditorCanvas)

}  // namespace starlight

