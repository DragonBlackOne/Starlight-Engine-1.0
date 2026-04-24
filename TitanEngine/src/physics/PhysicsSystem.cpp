#include "PhysicsSystem.hpp"
#include "Log.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <thread>

namespace titan {

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
    PhysicsSystem::~PhysicsSystem() { Shutdown(); }

    void PhysicsSystem::Initialize() {
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
    }

    void PhysicsSystem::Update(float dt) {
        if (m_physicsSystem) {
            m_collisionEvents.clear();
            m_physicsSystem->Update(dt, 1, m_tempAllocator, m_jobSystem);

            // Sync Jolt Bodies to TransformComponent
            auto activeScene = Engine::Get().GetSceneStack().Active();
            if (activeScene) {
                auto view = activeScene->GetRegistry().view<PhysicsComponent, TransformComponent>();
                auto& bodyInterface = m_physicsSystem->GetBodyInterface();
                for (auto entity : view) {
                    auto& phys = view.get<PhysicsComponent>(entity);
                    auto& trans = view.get<TransformComponent>(entity);
                    if (!phys.bodyID.IsInvalid()) {
                        JPH::RVec3 pos = bodyInterface.GetPosition(phys.bodyID);
                        JPH::Quat rot = bodyInterface.GetRotation(phys.bodyID);
                        trans.position = glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
                        trans.rotation = glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
                    }
                }
            }
        }
    }

    void PhysicsSystem::CreateBody(entt::entity entity, const JPH::Vec3& position, const JPH::Quat& rotation, const JPH::Vec3& halfExtents, JPH::EMotionType motionType, uint8_t layer) {
        auto& bodyInterface = m_physicsSystem->GetBodyInterface();
        
        JPH::BoxShapeSettings shapeSettings(halfExtents);
        JPH::Shape::ShapeResult result = shapeSettings.Create();
        
        JPH::BodyCreationSettings settings(result.Get(), position, rotation, motionType, layer);
        JPH::BodyID bodyID = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
        
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (activeScene) {
            activeScene->GetRegistry().emplace<PhysicsComponent>(entity, bodyID);
        }
        
        Log::Info("PhysicsBody added to entity.");
    }

    void PhysicsSystem::Shutdown() {
        if (m_physicsSystem) {
            delete m_physicsSystem;
            m_physicsSystem = nullptr;
        }
        delete m_contactListener;
        delete m_tempAllocator;
        delete m_jobSystem;
        delete m_bpLayerInterface;
        delete m_objVsBpFilter;
        delete m_objLayerPairFilter;
        if (JPH::Factory::sInstance) {
            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }
}
