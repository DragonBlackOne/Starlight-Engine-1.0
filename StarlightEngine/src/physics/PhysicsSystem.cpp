#include "PhysicsSystem.hpp"
#include "Log.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#pragma warning(pop)
#include <thread>

namespace starlight {

    // --- Jolt Interface Implementations ---
    class PhysicsSystem::BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
    public:
        BPLayerInterfaceImpl() {
            mObjectToBroadPhase[PhysicsLayers::NON_MOVING] = JPH::BroadPhaseLayer(0);
            mObjectToBroadPhase[PhysicsLayers::MOVING] = JPH::BroadPhaseLayer(1);
        }
        virtual JPH::uint GetNumBroadPhaseLayers() const override { return 2; }
        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return mObjectToBroadPhase[inLayer]; }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
            switch ((JPH::BroadPhaseLayer::Type)inLayer) {
                case 0: return "NON_MOVING";
                case 1: return "MOVING";
                default: return "INVALID";
            }
        }
#endif
    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[PhysicsLayers::NUM_LAYERS];
    };

    class PhysicsSystem::ObjectVsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
            switch (inLayer1) {
                case PhysicsLayers::NON_MOVING: return inLayer2 == JPH::BroadPhaseLayer(1);
                case PhysicsLayers::MOVING: return true;
                default: return false;
            }
        }
    };

    class PhysicsSystem::ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override {
            switch (inLayer1) {
                case PhysicsLayers::NON_MOVING: return inLayer2 == PhysicsLayers::MOVING;
                case PhysicsLayers::MOVING: return true;
                default: return false;
            }
        }
    };

    class PhysicsSystem::MyContactListener : public JPH::ContactListener {
    public:
        MyContactListener(std::vector<CollisionEvent>& events) : m_events(events) {}
        
        virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {
            (void)inManifold;
            (void)ioSettings;
            m_events.push_back({inBody1.GetID(), inBody2.GetID()});
        }
    private:
        std::vector<CollisionEvent>& m_events;
    };

    PhysicsSystem::PhysicsSystem() {}
    PhysicsSystem::~PhysicsSystem() { OnShutdown(); }

    bool PhysicsSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        m_tempAllocator = new JPH::TempAllocatorImpl(64 * 1024 * 1024);
        m_jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, (int)std::thread::hardware_concurrency() - 1);

        m_bpLayerInterface = new BPLayerInterfaceImpl();
        m_objVsBpFilter = new ObjectVsBroadPhaseLayerFilterImpl();
        m_objLayerPairFilter = new ObjectLayerPairFilterImpl();

        m_physicsSystem = new JPH::PhysicsSystem();
        m_physicsSystem->Init(8192, 0, 8192, 8192, *m_bpLayerInterface, *m_objVsBpFilter, *m_objLayerPairFilter);
        
        m_contactListener = new MyContactListener(m_collisionEvents);
        m_physicsSystem->SetContactListener(m_contactListener);

        Log::Info("Physics System (Jolt) Initialized.");
        return true;
    }

    void PhysicsSystem::OnFixedUpdate(float dt) {
        if (m_physicsSystem) {
            m_collisionEvents.clear();
            
            // Sync ECS transform changes to Kinematic bodies before physical step
            auto activeScene = Engine::Get().GetSceneStack().Active();
            if (activeScene) {
                auto& bodyInterface = m_physicsSystem->GetBodyInterface();
                auto kinematicView = activeScene->GetRegistry().view<PhysicsComponent, TransformComponent>();
                kinematicView.each([&bodyInterface](auto entity, auto& phys, auto& trans) {
                    (void)entity;
                    if (!phys.bodyID.IsInvalid() && bodyInterface.GetMotionType(phys.bodyID) == JPH::EMotionType::Kinematic) {
                        bodyInterface.SetPositionAndRotation(
                            phys.bodyID,
                            JPH::RVec3(trans.position.x, trans.position.y, trans.position.z),
                            JPH::Quat(trans.rotation.x, trans.rotation.y, trans.rotation.z, trans.rotation.w),
                            JPH::EActivation::Activate
                        );
                    }
                });
            }

            // Jolt 5.5.0: Update parameters optimization (using a single step)
            m_physicsSystem->Update(dt, 1, m_tempAllocator, m_jobSystem);

            // Sync Jolt Bodies to TransformComponent using EnTT 3.16.0 each()
            if (activeScene) {
                auto view = activeScene->GetRegistry().view<PhysicsComponent, TransformComponent>();
                auto& bodyInterface = m_physicsSystem->GetBodyInterface();
                
                view.each([&bodyInterface](auto& phys, auto& trans) {
                    if (!phys.bodyID.IsInvalid()) {
                        JPH::RVec3 pos;
                        JPH::Quat rot;
                        bodyInterface.GetPositionAndRotation(phys.bodyID, pos, rot);
                        
                        trans.position = glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
                        trans.rotation = glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
                    }
                });

                // Update CharacterVirtual characters dynamically
                auto charView = activeScene->GetRegistry().view<CharacterControllerComponent, TransformComponent>();
                charView.each([this, dt](entt::entity entity, auto& comp, auto& trans) {
                    (void)entity;
                    if (!comp.joltCharacter) {
                        JPH::CharacterVirtualSettings settings;
                        float capsuleHalfHeight = comp.height * 0.5f - comp.radius;
                        if (capsuleHalfHeight <= 0.0f) capsuleHalfHeight = 0.01f;
                        settings.mShape = new JPH::CapsuleShape(capsuleHalfHeight, comp.radius);
                        settings.mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -comp.radius);
                        settings.mMaxSlopeAngle = glm::radians(comp.maxSlopeAngle);
                        
                        auto character = new JPH::CharacterVirtual(&settings, JPH::RVec3(trans.position.x, trans.position.y, trans.position.z), JPH::Quat::sIdentity(), m_physicsSystem);
                        comp.joltCharacter = character;
                    }

                    JPH::CharacterVirtual* character = (JPH::CharacterVirtual*)comp.joltCharacter;
                    if (character) {
                        character->SetLinearVelocity(JPH::Vec3(comp.velocity.x, comp.velocity.y, comp.velocity.z));
                        
                        character->Update(dt, m_physicsSystem->GetGravity(), JPH::BroadPhaseLayerFilter(), JPH::ObjectLayerFilter(), JPH::BodyFilter(), JPH::ShapeFilter(), *m_tempAllocator);
                        
                        JPH::RVec3 pos = character->GetPosition();
                        JPH::Quat rot = character->GetRotation();
                        trans.position = glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
                        trans.rotation = glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
                        
                        comp.isGrounded = character->IsSupported();
                    }
                });
            }
        }
    }

    void PhysicsSystem::CreateBody(entt::entity entity, const JPH::Vec3& position, const JPH::Quat& rotation, const JPH::Vec3& halfExtents, JPH::EMotionType motionType, uint8_t layer) {
        auto& bodyInterface = m_physicsSystem->GetBodyInterface();
        
        JPH::BoxShapeSettings shapeSettings(halfExtents);
        JPH::Shape::ShapeResult result = shapeSettings.Create();
        
        JPH::BodyCreationSettings settings(result.Get(), position, rotation, motionType, layer);
        settings.mUserData = static_cast<JPH::uint64>(entity);
        JPH::BodyID bodyID = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
        
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (activeScene) {
            activeScene->GetRegistry().emplace<PhysicsComponent>(entity, bodyID);
        }
        
        Log::Info("PhysicsBody added to entity.");
    }

    void PhysicsSystem::DestroyBody(entt::entity entity) {
        if (!Engine::IsInitialized()) return;
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene || !m_physicsSystem) return;

        auto& registry = activeScene->GetRegistry();
        if (registry.any_of<PhysicsComponent>(entity)) {
            auto& phys = registry.get<PhysicsComponent>(entity);
            if (!phys.bodyID.IsInvalid()) {
                auto& bodyInterface = m_physicsSystem->GetBodyInterface();
                bodyInterface.RemoveBody(phys.bodyID);
                bodyInterface.DestroyBody(phys.bodyID);
                phys.bodyID = JPH::BodyID();
            }
        }
    }

    void PhysicsSystem::ApplyForce(entt::entity entity, const glm::vec3& force) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene || !m_physicsSystem) return;

        auto& registry = activeScene->GetRegistry();
        if (registry.any_of<PhysicsComponent>(entity)) {
            auto& phys = registry.get<PhysicsComponent>(entity);
            if (!phys.bodyID.IsInvalid()) {
                m_physicsSystem->GetBodyInterface().AddForce(phys.bodyID, JPH::Vec3(force.x, force.y, force.z));
            }
        }
    }

    void PhysicsSystem::ApplyImpulse(entt::entity entity, const glm::vec3& impulse) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene || !m_physicsSystem) return;

        auto& registry = activeScene->GetRegistry();
        if (registry.any_of<PhysicsComponent>(entity)) {
            auto& phys = registry.get<PhysicsComponent>(entity);
            if (!phys.bodyID.IsInvalid()) {
                m_physicsSystem->GetBodyInterface().AddImpulse(phys.bodyID, JPH::Vec3(impulse.x, impulse.y, impulse.z));
            }
        }
    }

    void PhysicsSystem::SetVelocity(entt::entity entity, const glm::vec3& velocity) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene || !m_physicsSystem) return;

        auto& registry = activeScene->GetRegistry();
        if (registry.any_of<PhysicsComponent>(entity)) {
            auto& phys = registry.get<PhysicsComponent>(entity);
            if (!phys.bodyID.IsInvalid()) {
                m_physicsSystem->GetBodyInterface().SetLinearAndAngularVelocity(
                    phys.bodyID, 
                    JPH::Vec3(velocity.x, velocity.y, velocity.z), 
                    JPH::Vec3(0, 0, 0)
                );
            }
        }
    }

    void PhysicsSystem::OnShutdown() {
        // Destroy all virtual characters
        if (Engine::IsInitialized()) {
            auto activeScene = Engine::Get().GetSceneStack().Active();
            if (activeScene) {
                auto view = activeScene->GetRegistry().view<CharacterControllerComponent>();
                view.each([](auto& comp) {
                    if (comp.joltCharacter) {
                        delete (JPH::CharacterVirtual*)comp.joltCharacter;
                        comp.joltCharacter = nullptr;
                    }
                });
            }
        }

        if (m_physicsSystem) {
            delete m_physicsSystem;
            m_physicsSystem = nullptr;
        }
        if (m_contactListener) {
            delete m_contactListener;
            m_contactListener = nullptr;
        }
        if (m_tempAllocator) {
            delete m_tempAllocator;
            m_tempAllocator = nullptr;
        }
        if (m_jobSystem) {
            delete m_jobSystem;
            m_jobSystem = nullptr;
        }
        if (m_bpLayerInterface) {
            delete m_bpLayerInterface;
            m_bpLayerInterface = nullptr;
        }
        if (m_objVsBpFilter) {
            delete m_objVsBpFilter;
            m_objVsBpFilter = nullptr;
        }
        if (m_objLayerPairFilter) {
            delete m_objLayerPairFilter;
            m_objLayerPairFilter = nullptr;
        }
        if (JPH::Factory::sInstance) {
            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }

    void PhysicsSystem::GetComponentAccess(std::vector<std::type_index>& reads, std::vector<std::type_index>& writes) const {
        reads.push_back(typeid(PhysicsComponent));
        writes.push_back(typeid(TransformComponent));
    }
}
