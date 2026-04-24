// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "components.h"

ECS_COMPONENT_DECLARE(Transform);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(Sprite);
ECS_COMPONENT_DECLARE(Mesh);

ECS_COMPONENT_DECLARE(AABB);

void components_register(ecs_world_t* world) {
    ECS_COMPONENT_DEFINE(world, Transform);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_COMPONENT_DEFINE(world, Sprite);
    ECS_COMPONENT_DEFINE(world, Mesh);
    ECS_COMPONENT_DEFINE(world, AABB);
}
