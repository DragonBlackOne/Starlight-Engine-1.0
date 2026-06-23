#pragma once
#include "EngineSystem.hpp"
#include <memory>

namespace starlight {

class OctreeSystem;

class CameraSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "CameraSystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class ClothSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "ClothSystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class HierarchySystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "HierarchySystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class LODSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "LODSystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class VehicleSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "VehicleSystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class MeshDeformSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "MeshDeformSystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class AISystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "AISystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
};

class ConsoleSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    const char* GetName() const override { return "ConsoleSystem"; }
    bool IsMainThreadOnly() const override { return true; }
};

class OctreeSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "OctreeSystem"; }
    void GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const override;
    OctreeSystem& GetOctree() { return *m_octree; }

private:
    std::unique_ptr<OctreeSystem> m_octree;
};

class PickerSystemAdapter : public ISystem {
public:
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "PickerSystem"; }
    bool IsMainThreadOnly() const override { return true; }
    entt::entity GetPicked() const { return m_picked; }

private:
    entt::entity m_picked = entt::null;
};

}
