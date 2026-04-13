#include "ecs_systems.h"
#include "job_system.h"
#include "physics.h"
#include "camera.h"
#include <SDL2/SDL_atomic.h>

#include "components.h"

typedef struct {
    RigidBody rb;
} PhysicsComponent;

// Job para processamento de fisica em paralelo
typedef struct {
    PhysicsComponent* components;
    int count;
    float dt;
} PhysicsJobData;

static void physics_job(void* data) {
    PhysicsJobData* job_data = (PhysicsJobData*)data;
    for (int i = 0; i < job_data->count; ++i) {
        physics_integrate(&job_data->components[i].rb, job_data->dt);
    }
}

ECS_COMPONENT_DECLARE(PhysicsComponent);

static void PhysicsSystem(ecs_iter_t *it) {
    Transform *t = ecs_field(it, Transform, 1);
    PhysicsComponent *p = ecs_field(it, PhysicsComponent, 2);

    SDL_atomic_t counter;
    SDL_AtomicSet(&counter, 0);

    // Dividir entidades em chunks para o Job System
    int chunk_size = 64;
    for (int i = 0; i < it->count; i += chunk_size) {
        int current_chunk = (it->count - i < chunk_size) ? (it->count - i) : chunk_size;
        
        // Nota: Em uma versao real, usaríamos uma Arena para alocar o JobData
        PhysicsJobData* job_data = (PhysicsJobData*)malloc(sizeof(PhysicsJobData));
        job_data->components = &p[i];
        job_data->count = current_chunk;
        job_data->dt = it->delta_time;

        job_system_push(physics_job, job_data, &counter);
    }

    job_system_wait(&counter);
    
    // Sincronizar Transform com o resultado da Fisica
    for (int i = 0; i < it->count; i++) {
        glm_vec3_copy(p[i].rb.position, t[i].position);
    }
}

static void VelocitySystem(ecs_iter_t *it) {
    Transform *t = ecs_field(it, Transform, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    for (int i = 0; i < it->count; i++) {
        glm_vec3_muladds(v[i].velocity, it->delta_time, t[i].position);
    }
}

void ecs_register_systems(ecs_world_t* world) {
    ECS_COMPONENT_DEFINE(world, PhysicsComponent);

    // Sistema de atualizacao de Transform baseada na Fisica
    // Este sistema roda no Job System para aproveitar multinucleos
    ECS_SYSTEM(world, PhysicsSystem, EcsOnUpdate, Transform, PhysicsComponent);

    // Sistema de velocidade simples (Data-Driven Ready)
    ECS_SYSTEM(world, VelocitySystem, EcsOnUpdate, Transform, Velocity);
}
