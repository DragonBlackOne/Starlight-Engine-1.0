// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>

namespace Vamos {

    enum class BodyType {
        Static,
        Kinematic,
        Dynamic
    };

    enum class ShapeType {
        Box,
        Sphere
    };

    struct RigidBody3DComponent {
        BodyType type = BodyType::Dynamic;
        ShapeType shape = ShapeType::Box;
        float mass = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        
        // Dados de tempo de execução
        JPH::BodyID bodyID;
        bool initialized = false;
    };
}
