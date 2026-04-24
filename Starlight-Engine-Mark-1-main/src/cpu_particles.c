// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "cpu_particles.h"
#include "job_system.h"
#include <glad/glad.h>

typedef struct {
    SLF_ParticleSystem* sys;
    int start_idx;
    int end_idx;
    float dt;
} ParticleJobChunk;

#define SPH_SMOOTHING_RADIUS 1.5f
#define SPH_STIFFNESS 50.0f
#define SPH_REST_DENSITY 2.0f
#define SPH_VISCOSITY 0.5f
#define SPH_MASS 1.0f

static void particle_sph_density_job(void* data) {
    ParticleJobChunk* chunk = (ParticleJobChunk*)data;
    SLF_ParticleSystem* sys = chunk->sys;
    float h = SPH_SMOOTHING_RADIUS;
    float h2 = h * h;
    float mass_poly6 = (315.0f / (64.0f * 3.1415f * powf(h, 9.0f))) * SPH_MASS;

    for(int i = chunk->start_idx; i < chunk->end_idx; ++i) {
        if(sys->particles[i].life <= 0.0f) continue;
        float density = 0.0f;
        for(int j = 0; j < sys->active_count; ++j) {
            if (sys->particles[j].life <= 0.0f) continue;
            vec3 diff; glm_vec3_sub(sys->particles[i].position, sys->particles[j].position, diff);
            float r2 = glm_vec3_norm2(diff);
            if (r2 < h2) {
                float h2_r2 = h2 - r2;
                density += mass_poly6 * h2_r2 * h2_r2 * h2_r2;
            }
        }
        if (density < 0.001f) density = 0.001f;
        sys->particles[i].density = density;
        sys->particles[i].pressure = SPH_STIFFNESS * (density - SPH_REST_DENSITY);
    }
}

static void particle_sph_force_job(void* data) {
    ParticleJobChunk* chunk = (ParticleJobChunk*)data;
    SLF_ParticleSystem* sys = chunk->sys;
    float h = SPH_SMOOTHING_RADIUS;
    float h2 = h * h;
    float spiky_grad = -45.0f / (3.1415f * powf(h, 6.0f));
    float visc_lap = 45.0f / (3.1415f * powf(h, 6.0f));

    for(int i = chunk->start_idx; i < chunk->end_idx; ++i) {
        if(sys->particles[i].life <= 0.0f) continue;
        vec3 force_pressure = {0,0,0};
        vec3 force_viscosity = {0,0,0};
        
        for(int j = 0; j < sys->active_count; ++j) {
            if (i == j || sys->particles[j].life <= 0.0f) continue;
            vec3 diff; glm_vec3_sub(sys->particles[i].position, sys->particles[j].position, diff);
            float r2 = glm_vec3_norm2(diff);
            if (r2 < h2 && r2 > 0.0001f) {
                float r = sqrtf(r2);
                float h_r = h - r;
                
                // Pressure force
                float p_term = SPH_MASS * (sys->particles[i].pressure + sys->particles[j].pressure) / (2.0f * sys->particles[j].density);
                float kernel = spiky_grad * h_r * h_r;
                vec3 norm_diff; glm_vec3_scale(diff, 1.0f / r, norm_diff);
                vec3 p_force; glm_vec3_scale(norm_diff, p_term * kernel, p_force);
                glm_vec3_add(force_pressure, p_force, force_pressure);
                
                // Viscosity force
                vec3 vel_diff; glm_vec3_sub(sys->particles[j].velocity, sys->particles[i].velocity, vel_diff);
                float v_term = SPH_MASS * SPH_VISCOSITY / sys->particles[j].density;
                float v_kernel = visc_lap * h_r;
                vec3 v_force; glm_vec3_scale(vel_diff, v_term * v_kernel, v_force);
                glm_vec3_add(force_viscosity, v_force, force_viscosity);
            }
        }
        glm_vec3_add(force_pressure, force_viscosity, sys->particles[i].force);
        vec3 gravity = {0.0f, -9.81f * sys->particles[i].density * 0.1f, 0.0f};
        glm_vec3_add(sys->particles[i].force, gravity, sys->particles[i].force);
    }
}

static void particle_update_job(void* data) {
    ParticleJobChunk* chunk = (ParticleJobChunk*)data;
    SLF_ParticleSystem* sys = chunk->sys;
    float dt = chunk->dt;
    
    for(int i = chunk->start_idx; i < chunk->end_idx; ++i) {
        if(sys->particles[i].life > 0.0f) {
            sys->particles[i].life -= dt;
            
            // F = ma -> a = F / density (for SPH)
            vec3 acc; glm_vec3_scale(sys->particles[i].force, 1.0f / sys->particles[i].density, acc);
            
            vec3 dv; glm_vec3_scale(acc, dt, dv);
            glm_vec3_add(sys->particles[i].velocity, dv, sys->particles[i].velocity);
            
            vec3 dp; glm_vec3_scale(sys->particles[i].velocity, dt, dp);
            glm_vec3_add(sys->particles[i].position, dp, sys->particles[i].position);
            
            // Floor collision (simple bounds)
            if (sys->particles[i].position[1] < 0.0f) {
                sys->particles[i].position[1] = 0.01f;
                sys->particles[i].velocity[1] *= -0.5f; // Bouncy
            }
            
            sys->particles[i].color[3] = sys->particles[i].life / sys->particles[i].max_life;
        }
    }
}

bool starlight_particle_sys_init(SLF_ParticleSystem* sys) {
    sys->active_count = 0;
    
    glGenVertexArrays(1, &sys->vao);
    glGenBuffers(1, &sys->vbo);
    
    glBindVertexArray(sys->vao);
    glBindBuffer(GL_ARRAY_BUFFER, sys->vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_CPU_PARTICLES * sizeof(SLF_Particle), NULL, GL_DYNAMIC_DRAW);
    
    glBindVertexArray(0);
    return true;
}

void starlight_particle_sys_emit(SLF_ParticleSystem* sys, vec3 position, vec3 velocity, vec4 color, float life, float size) {
    if(sys->active_count >= MAX_CPU_PARTICLES) return;
    
    SLF_Particle* p = &sys->particles[sys->active_count++];
    glm_vec3_copy(position, p->position);
    glm_vec3_copy(velocity, p->velocity);
    glm_vec3_zero(p->force);
    glm_vec4_copy(color, p->color);
    p->life = life;
    p->max_life = life;
    p->size = size;
    p->density = 0.0f;
    p->pressure = 0.0f;
}

void starlight_particle_sys_update(SLF_ParticleSystem* sys, float dt) {
    if(sys->active_count == 0) return;
    
    int num_chunks = 4;
    int chunk_size = sys->active_count / num_chunks;
    if(chunk_size < 100) { num_chunks = 1; chunk_size = sys->active_count; }
    
    SDL_atomic_t sync_counter;
    ParticleJobChunk chunks[16]; 
    if(num_chunks > 16) num_chunks = 16;
    
    // Pass 1: Density & Pressure
    SDL_AtomicSet(&sync_counter, 0);
    for (int i = 0; i < num_chunks; ++i) {
        chunks[i].sys = sys;
        chunks[i].start_idx = i * chunk_size;
        chunks[i].end_idx = (i == num_chunks - 1) ? sys->active_count : (i + 1) * chunk_size;
        chunks[i].dt = dt;
        job_system_push(particle_sph_density_job, &chunks[i], &sync_counter);
    }
    job_system_wait(&sync_counter);
    
    // Pass 2: Force (Viscosity & Pressure Gradient)
    SDL_AtomicSet(&sync_counter, 0);
    for (int i = 0; i < num_chunks; ++i) {
        job_system_push(particle_sph_force_job, &chunks[i], &sync_counter);
    }
    job_system_wait(&sync_counter);

    // Pass 3: Integration
    SDL_AtomicSet(&sync_counter, 0);
    for (int i = 0; i < num_chunks; ++i) {
        job_system_push(particle_update_job, &chunks[i], &sync_counter);
    }
    job_system_wait(&sync_counter);
    
    // Post-Integration Cleanup
    for(int i = 0; i < sys->active_count; ++i) {
        if(sys->particles[i].life <= 0.0f) {
            sys->particles[i] = sys->particles[sys->active_count - 1];
            sys->active_count--;
            i--;
        }
    }
    
    // Update GPU Buffer
    glBindBuffer(GL_ARRAY_BUFFER, sys->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sys->active_count * sizeof(SLF_Particle), sys->particles);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void starlight_particle_sys_destroy(SLF_ParticleSystem* sys) {
    if (sys->vao) glDeleteVertexArrays(1, &sys->vao);
    if (sys->vbo) glDeleteBuffers(1, &sys->vbo);
    sys->vao = 0;
    sys->vbo = 0;
    sys->active_count = 0;
}
