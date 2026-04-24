// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef PHYSICS_H
#define PHYSICS_H

#include <cglm/cglm.h>
#include <stdbool.h>

// Tipos de Collider
typedef enum {
    COLLIDER_SPHERE,
    COLLIDER_AABB,
    COLLIDER_PLANE
} ColliderType;

// Corpo Rigido
typedef struct {
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    vec3 force_accumulator;
    float mass;
    float inverse_mass;
    float restitution;   // bounciness (0-1)
    float friction;
    bool is_static;
    bool use_gravity;
} RigidBody;

// Collider Esfera
typedef struct {
    float radius;
} SphereCollider;

// Collider AABB
typedef struct {
    vec3 half_extents;
} AABBCollider;

// Collider Plano Infinito
typedef struct {
    vec3 normal;
    float distance;
} PlaneCollider;

// Info do Contato
typedef struct {
    vec3 point;
    vec3 normal;
    float penetration;
    bool has_collision;
} ContactInfo;

// API
void physics_rigidbody_init(RigidBody* rb, float mass, bool is_static);
void physics_apply_force(RigidBody* rb, vec3 force);
void physics_integrate(RigidBody* rb, float dt);

// Deteccao de colisao
ContactInfo physics_sphere_vs_plane(vec3 sphere_pos, float radius, vec3 plane_normal, float plane_dist);
ContactInfo physics_sphere_vs_sphere(vec3 pos_a, float radius_a, vec3 pos_b, float radius_b);
ContactInfo physics_aabb_vs_aabb(vec3 pos_a, vec3 half_a, vec3 pos_b, vec3 half_b);

// Physics Raycast for Audio Occlusion / Line of Sight
bool physics_ray_vs_aabb(vec3 ray_origin, vec3 ray_dir, vec3 aabb_min, vec3 aabb_max, float* hit_dist);

// Resolucao
void physics_resolve_collision(RigidBody* a, RigidBody* b, ContactInfo* contact);

// --- Kinematic Character Controller (KCC) ---
typedef struct {
    vec3 position;
    vec3 velocity;        // For gravity/jumping
    float radius;         // Cylinder/Capsule radius
    float height;         // Total height
    float step_height;    // Max stair step height
    float max_slope;      // Max slope angle (radians) before sliding
    bool is_grounded;
} KCC;

void physics_kcc_init(KCC* kcc, vec3 start_pos, float radius, float height);
void physics_kcc_move(
    KCC* kcc, 
    vec3 movement, 
    PlaneCollider* planes, int num_planes,
    SphereCollider* spheres, vec3* sphere_positions, int num_spheres,
    AABBCollider* aabbs, vec3* aabb_positions, int num_aabbs
);

#endif
