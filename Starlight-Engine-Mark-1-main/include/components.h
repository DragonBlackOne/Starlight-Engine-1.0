// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef STARLIGHT_COMPONENTS_H
#define STARLIGHT_COMPONENTS_H

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <flecs.h>
#include "camera.h"  // Provide AABB

// Componente Transform AAA
typedef struct {
    vec3 position;
    vec3 rotation;
    vec3 scale;
} Transform;

typedef struct {
    vec3 velocity;
} Velocity;

typedef struct {
    vec4 color;
} Sprite;

typedef struct {
    char mesh_path[128];
    GLuint texture_id;
} Mesh;

// Macros para registrar componentes facilmente
extern ECS_COMPONENT_DECLARE(Transform);
extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Sprite);
extern ECS_COMPONENT_DECLARE(Mesh);
extern ECS_COMPONENT_DECLARE(AABB);

void components_register(ecs_world_t* world);

#endif
