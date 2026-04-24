// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - 2D Physics Module (Box2D v3)
// Licença: MIT | Origem: Erin Catto
// ============================================================

#include <box2d/box2d.h>
#include <vector>
#include <functional>
#include "Log.hpp"

namespace Vamos {
    class Physics2DWorld {
    public:
        Physics2DWorld(float gravityX = 0.0f, float gravityY = -9.81f) {
            b2WorldDef worldDef = b2DefaultWorldDef();
            worldDef.gravity = {gravityX, gravityY};
            worldId = b2CreateWorld(&worldDef);
            VAMOS_CORE_INFO("Physics2D: Box2D v3 inicializado (gravidade: {0}, {1})", gravityX, gravityY);
        }

        ~Physics2DWorld() {
            b2DestroyWorld(worldId);
        }

        void Step(float timeStep = 1.0f / 60.0f, int subStepCount = 4) {
            b2World_Step(worldId, timeStep, subStepCount);
        }

        b2BodyId CreateStaticBody(float x, float y) {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_staticBody;
            bodyDef.position = {x, y};
            return b2CreateBody(worldId, &bodyDef);
        }

        b2BodyId CreateDynamicBody(float x, float y) {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = {x, y};
            return b2CreateBody(worldId, &bodyDef);
        }

        b2BodyId CreateKinematicBody(float x, float y) {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_kinematicBody;
            bodyDef.position = {x, y};
            return b2CreateBody(worldId, &bodyDef);
        }

        void AddBoxShape(b2BodyId body, float halfWidth, float halfHeight, float density = 1.0f, float friction = 0.3f) {
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = density;
            shapeDef.material.friction = friction;
            b2Polygon box = b2MakeBox(halfWidth, halfHeight);
            b2CreatePolygonShape(body, &shapeDef, &box);
        }

        void AddCircleShape(b2BodyId body, float radius, float density = 1.0f, float friction = 0.3f) {
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = density;
            shapeDef.material.friction = friction;
            b2Circle circle = {{0.0f, 0.0f}, radius};
            b2CreateCircleShape(body, &shapeDef, &circle);
        }

        b2Vec2 GetPosition(b2BodyId body) const {
            return b2Body_GetPosition(body);
        }

        b2Rot GetRotation(b2BodyId body) const {
            return b2Body_GetRotation(body);
        }

        b2WorldId GetWorldId() const { return worldId; }

    private:
        b2WorldId worldId;
    };

    // Componente ECS para física 2D
    struct Physics2DBody {
        b2BodyId bodyId;
        bool isSensor = false;
    };
}
