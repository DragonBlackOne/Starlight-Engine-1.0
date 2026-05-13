// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <vector>
#include <memory>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

// Jolt Includes
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#pragma warning(pop)

#include "EngineSystem.hpp"

namespace starlight {
    namespace PhysicsLayers {
        static constexpr uint8_t NON_MOVING = 0;
        static constexpr uint8_t MOVING = 1;
        static constexpr uint8_t NUM_LAYERS = 2;
    }

    class PhysicsSystem : public ISystem {
    public:
        PhysicsSystem();
        ~PhysicsSystem();

        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnFixedUpdate(float dt) override;
        const char* GetName() const override { return "PhysicsSystem"; }

        void CreateBody(entt::entity entity, const JPH::Vec3& position, const JPH::Quat& rotation, const JPH::Vec3& halfExtents, JPH::EMotionType motionType, uint8_t layer);
        
        // Gameplay Mechanics
        void ApplyForce(entt::entity entity, const glm::vec3& force);
        void ApplyImpulse(entt::entity entity, const glm::vec3& impulse);
        void SetVelocity(entt::entity entity, const glm::vec3& velocity);
        
        JPH::BodyInterface& GetBodyInterface() { return m_physicsSystem->GetBodyInterface(); }
        JPH::PhysicsSystem* GetSystem() { return m_physicsSystem; }

        struct CollisionEvent {
            JPH::BodyID b1, b2;
        };
        std::vector<CollisionEvent>& GetCollisionEvents() { return m_collisionEvents; }

    private:
        JPH::PhysicsSystem* m_physicsSystem = nullptr;
        JPH::TempAllocatorImpl* m_tempAllocator = nullptr;
        JPH::JobSystemThreadPool* m_jobSystem = nullptr;

        class MyContactListener;
        MyContactListener* m_contactListener = nullptr;
        std::vector<CollisionEvent> m_collisionEvents;

        // Implementation-specific interfaces
        class BPLayerInterfaceImpl;
        class ObjectVsBroadPhaseLayerFilterImpl;
        class ObjectLayerPairFilterImpl;

        BPLayerInterfaceImpl* m_bpLayerInterface = nullptr;
        ObjectVsBroadPhaseLayerFilterImpl* m_objVsBpFilter = nullptr;
        ObjectLayerPairFilterImpl* m_objLayerPairFilter = nullptr;
    };
}
