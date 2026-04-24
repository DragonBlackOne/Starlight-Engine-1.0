// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef ECS_SYSTEMS_H
#define ECS_SYSTEMS_H

#include "components.h"
#include <flecs.h>
#include <stdio.h>

// --- COMPONENTES ECS ---
typedef struct {
    float x, y, z;
} Position;

// --- SISTEMAS ECS ---
static void MoveSystem(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].velocity[0] * it->delta_time;
        p[i].y += v[i].velocity[1] * it->delta_time;
        p[i].z += v[i].velocity[2] * it->delta_time;
    }
}

// Inicializa o ECS World com componentes e sistemas registrados
static inline ecs_world_t* ecs_setup(int threads) {
    ecs_world_t *world = ecs_init();
    ecs_set_threads(world, threads);

    ECS_COMPONENT(world, Position);
    // ECS_COMPONENT(world, Velocity); // Use Velocity from components.h
    ECS_SYSTEM(world, MoveSystem, EcsOnUpdate, Position, Velocity);

    // Spawn test entity
    ecs_entity_t player = ecs_new_id(world);
    ecs_set_name(world, player, "TEST_PLAYER");

    Position p = {10.0f, 0.0f, 0.0f};
    Velocity v = {{2.5f, 0.0f, 0.0f}};
    ecs_set_id(world, player, ecs_id(Position), sizeof(Position), &p);
    ecs_set_id(world, player, ecs_id(Velocity), sizeof(Velocity), &v);

    printf("[ECS] World inicializado com %d threads.\n", threads);
    return world;
}

#endif
