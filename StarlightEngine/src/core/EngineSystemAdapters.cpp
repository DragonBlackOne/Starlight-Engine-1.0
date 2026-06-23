#include "EngineSystemAdapters.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include "Components.hpp"
#include "AIComponent.hpp"
#include "LODComponent.hpp"
#include "InputSystem.hpp"
#include "Renderer.hpp"

#include "CameraSystem.hpp"
#include "ClothSystem.hpp"
#include "HierarchySystem.hpp"
#include "LODSystem.hpp"
#include "VehicleSystem.hpp"
#include "MeshDeformSystem.hpp"
#include "AISystem.hpp"
#include "ConsoleSystem.hpp"
#include "OctreeSystem.hpp"
#include "PickerSystem.hpp"

#include "imgui.h"

namespace starlight {

// ============================================================================
// CameraSystemAdapter
// ============================================================================

bool CameraSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("CameraSystemAdapter: Initialized.");
    return true;
}

void CameraSystemAdapter::OnShutdown() {
    Log::Info("CameraSystemAdapter: Shutdown.");
}

void CameraSystemAdapter::OnUpdate(float /*dt*/) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    auto& window = Engine::Get().GetWindow();
    float aspect = (float)window.GetWidth() / (float)window.GetHeight();

    CameraSystem::Update(activeScene->GetRegistry(), aspect);
}

// ============================================================================
// ClothSystemAdapter
// ============================================================================

bool ClothSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("ClothSystemAdapter: Initialized.");
    return true;
}

void ClothSystemAdapter::OnShutdown() {
    Log::Info("ClothSystemAdapter: Shutdown.");
}

void ClothSystemAdapter::OnUpdate(float dt) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    ClothSystem::Update(activeScene->GetRegistry(), dt);
}

// ============================================================================
// HierarchySystemAdapter
// ============================================================================

bool HierarchySystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("HierarchySystemAdapter: Initialized.");
    return true;
}

void HierarchySystemAdapter::OnShutdown() {
    Log::Info("HierarchySystemAdapter: Shutdown.");
}

void HierarchySystemAdapter::OnUpdate(float /*dt*/) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    HierarchySystem::Update(activeScene->GetRegistry());
}

// ============================================================================
// LODSystemAdapter
// ============================================================================

bool LODSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("LODSystemAdapter: Initialized.");
    return true;
}

void LODSystemAdapter::OnShutdown() {
    Log::Info("LODSystemAdapter: Shutdown.");
}

void LODSystemAdapter::OnUpdate(float /*dt*/) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    LODSystem::Update(activeScene->GetRegistry());
}

// ============================================================================
// VehicleSystemAdapter
// ============================================================================

bool VehicleSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("VehicleSystemAdapter: Initialized.");
    return true;
}

void VehicleSystemAdapter::OnShutdown() {
    Log::Info("VehicleSystemAdapter: Shutdown.");
}

void VehicleSystemAdapter::OnUpdate(float dt) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    VehicleSystem::Update(activeScene->GetRegistry(), dt);
}

// ============================================================================
// MeshDeformSystemAdapter
// ============================================================================

bool MeshDeformSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("MeshDeformSystemAdapter: Initialized.");
    return true;
}

void MeshDeformSystemAdapter::OnShutdown() {
    Log::Info("MeshDeformSystemAdapter: Shutdown.");
}

void MeshDeformSystemAdapter::OnUpdate(float dt) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    MeshDeformSystem::Update(activeScene->GetRegistry(), dt);
}

// ============================================================================
// AISystemAdapter
// ============================================================================

bool AISystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("AISystemAdapter: Initialized.");
    return true;
}

void AISystemAdapter::OnShutdown() {
    Log::Info("AISystemAdapter: Shutdown.");
}

void AISystemAdapter::OnUpdate(float dt) {
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    AISystem::Update(activeScene->GetRegistry(), dt);
}

// ============================================================================
// ConsoleSystemAdapter
// ============================================================================

bool ConsoleSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("ConsoleSystemAdapter: Initialized (press F1 to toggle).");
    return true;
}

void ConsoleSystemAdapter::OnShutdown() {
    Log::Info("ConsoleSystemAdapter: Shutdown.");
}

void ConsoleSystemAdapter::OnUpdate(float /*dt*/) {
    auto* input = Engine::Get().GetSystem<InputSystem>();
    if (input && input->IsActionJustPressed("Console")) {
        ConsoleSystem::Get().Toggle();
    }
}

void ConsoleSystemAdapter::OnRender() {
    auto& console = ConsoleSystem::Get();
    if (!console.IsVisible()) return;

    ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Console")) {
        ImGui::End();
        return;
    }

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                      false, ImGuiWindowFlags_HorizontalScrollbar);
    const auto& logs = console.GetLogs();
    for (const auto& line : logs) {
        ImGui::TextWrapped("%s", line.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    static char inputBuf[256] = "";
    ImGui::Separator();
    bool reclaimFocus = false;
    if (ImGui::InputText("##ConsoleInput", inputBuf, sizeof(inputBuf),
                         ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory)) {
        std::string cmd(inputBuf);
        if (!cmd.empty()) {
            console.Execute(cmd);
        }
        inputBuf[0] = '\0';
        reclaimFocus = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Submit")) {
        std::string cmd(inputBuf);
        if (!cmd.empty()) {
            console.Execute(cmd);
        }
        inputBuf[0] = '\0';
        reclaimFocus = true;
    }
    ImGui::End();

    if (reclaimFocus) {
        ImGui::SetKeyboardFocusHere(-1);
    }
}

// ============================================================================
// OctreeSystemAdapter
// ============================================================================

bool OctreeSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    AABB worldBounds;
    worldBounds.min = glm::vec3(-10000.0f);
    worldBounds.max = glm::vec3(10000.0f);
    m_octree = std::make_unique<OctreeSystem>(worldBounds);
    Log::Info("OctreeSystemAdapter: Initialized (world bounds 20km).");
    return true;
}

void OctreeSystemAdapter::OnShutdown() {
    m_octree.reset();
    Log::Info("OctreeSystemAdapter: Shutdown.");
}

void OctreeSystemAdapter::OnUpdate(float /*dt*/) {
    if (!m_octree) return;
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) {
        m_octree->Clear();
        return;
    }

    m_octree->Clear();
    auto& registry = activeScene->GetRegistry();
    auto view = registry.view<TransformComponent, MeshComponent>();
    for (auto entity : view) {
        const auto& transform = view.get<TransformComponent>(entity);
        const auto& mesh = view.get<MeshComponent>(entity);
        const float r = mesh.boundingRadius;
        AABB aabb;
        aabb.min = transform.position - glm::vec3(r);
        aabb.max = transform.position + glm::vec3(r);
        m_octree->Insert(entity, aabb);
    }
}

// ============================================================================
// PickerSystemAdapter
// ============================================================================

bool PickerSystemAdapter::OnInitialize(const EngineContext& /*context*/) {
    Log::Info("PickerSystemAdapter: Initialized (click to pick 3D entities).");
    return true;
}

void PickerSystemAdapter::OnShutdown() {
    Log::Info("PickerSystemAdapter: Shutdown.");
}

void PickerSystemAdapter::OnUpdate(float /*dt*/) {
    auto* input = Engine::Get().GetSystem<InputSystem>();
    if (!input || !input->IsActionJustPressed("MouseLeft")) {
        return;
    }

    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    auto* octreeAdapter = Engine::Get().GetSystem<OctreeSystemAdapter>();
    if (!octreeAdapter) return;

    auto* renderer = Engine::Get().GetSystem<Renderer>();
    if (!renderer) return;

    const auto& view = renderer->GetViewMatrix();
    const auto& proj = renderer->GetProjectionMatrix();
    const auto& window = Engine::Get().GetWindow();
    const glm::vec2 mouse = input->GetMousePosition();

    Ray ray = PickerSystem::ScreenPointToRay(
        (int)mouse.x, (int)mouse.y, window.GetWidth(), window.GetHeight(), view, proj);

    m_picked = PickerSystem::Pick(ray, activeScene->GetRegistry(), octreeAdapter->GetOctree());

    if (m_picked != entt::null) {
        std::string tagName = "Entity";
        if (activeScene->GetRegistry().all_of<TagComponent>(m_picked)) {
            tagName = activeScene->GetRegistry().get<TagComponent>(m_picked).tag;
        }
        Log::Info("PickerSystem: Picked entity {} (tag='{}').",
                  static_cast<uint32_t>(m_picked), tagName);
    }
}

void CameraSystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    reads.push_back(typeid(TransformComponent));
    writes.push_back(typeid(CameraComponent));
}

void ClothSystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    (void)reads;
    writes.push_back(typeid(ClothComponent));
}

void HierarchySystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    (void)reads;
    writes.push_back(typeid(TransformComponent));
    writes.push_back(typeid(RelationshipComponent));
}

void LODSystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    (void)reads;
    writes.push_back(typeid(MeshComponent));
    writes.push_back(typeid(LODComponent));
}

void VehicleSystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    reads.push_back(typeid(TransformComponent));
    reads.push_back(typeid(PhysicsComponent));
    writes.push_back(typeid(VehicleComponent));
}

void MeshDeformSystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    (void)reads;
    writes.push_back(typeid(MeshDeformComponent));
}

void AISystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    reads.push_back(typeid(AIComponent));
    writes.push_back(typeid(AIComponent));
    writes.push_back(typeid(TransformComponent));
}

void OctreeSystemAdapter::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
    (void)writes;
    reads.push_back(typeid(TransformComponent));
    reads.push_back(typeid(MeshComponent));
}

}

