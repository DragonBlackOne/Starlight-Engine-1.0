// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef CPU_PARTICLES_H
#define CPU_PARTICLES_H

#include <stdbool.h>
#include <cglm/cglm.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#define MAX_CPU_PARTICLES 10000

typedef struct {
    vec3 position;
    vec3 velocity;
    vec3 force;       // For SPH density/pressure
    vec4 color;
    float life;
    float max_life;
    float size;
    float density;    // SPH
    float pressure;   // SPH
} SLF_Particle;

typedef struct {
    SLF_Particle particles[MAX_CPU_PARTICLES];
    int active_count;
    GLuint vbo;
    GLuint vao;
} SLF_ParticleSystem;

bool starlight_particle_sys_init(SLF_ParticleSystem* sys);
void starlight_particle_sys_emit(SLF_ParticleSystem* sys, vec3 position, vec3 velocity, vec4 color, float life, float size);
void starlight_particle_sys_update(SLF_ParticleSystem* sys, float dt);
void starlight_particle_sys_destroy(SLF_ParticleSystem* sys);

#endif
