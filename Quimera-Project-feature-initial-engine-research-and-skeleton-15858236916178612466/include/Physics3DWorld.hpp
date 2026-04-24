// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <iostream>
#include <memory>
#include <thread>

namespace Vamos {

    namespace Layers {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    };

    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
    public:
        BPLayerInterfaceImpl() {
            mObjectToBroadPhase[Layers::NON_MOVING] = JPH::BroadPhaseLayer(0);
            mObjectToBroadPhase[Layers::MOVING] = JPH::BroadPhaseLayer(1);
        }
        virtual JPH::uint GetNumBroadPhaseLayers() const override { return 2; }
        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return mObjectToBroadPhase[inLayer]; }
        
        virtual const char * GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
            switch ((JPH::BroadPhaseLayer::Type)inLayer) {
                case 0: return "NON_MOVING";
                case 1: return "MOVING";
                default: return "INVALID";
            }
        }

    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
            switch (inLayer1) {
                case Layers::NON_MOVING: return inLayer2 == JPH::BroadPhaseLayer(1);
                case Layers::MOVING: return true;
                default: return false;
            }
        }
    };

    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
            switch (inObject1) {
                case Layers::NON_MOVING: return inObject2 == Layers::MOVING;
                case Layers::MOVING: return true;
                default: return false;
            }
        }
    };

    class Physics3DWorld {
    public:
        Physics3DWorld() {
            temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
            job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, (int)std::thread::hardware_concurrency() - 1);
            
            physics_system.Init(1024, 0, 1024, 1024, bp_layer_interface, obj_vs_bp_filter, obj_vs_obj_filter);
        }

        void Step(float dt = 1.0f / 60.0f) {
            physics_system.Update(dt, 1, temp_allocator.get(), job_system.get());
        }

        JPH::BodyInterface& GetBodyInterface() { return physics_system.GetBodyInterface(); }

    private:
        JPH::PhysicsSystem physics_system;
        std::unique_ptr<JPH::TempAllocatorImpl> temp_allocator;
        std::unique_ptr<JPH::JobSystemThreadPool> job_system;
        
        BPLayerInterfaceImpl bp_layer_interface;
        ObjectVsBroadPhaseLayerFilterImpl obj_vs_bp_filter;
        ObjectLayerPairFilterImpl obj_vs_obj_filter;
    };
}
