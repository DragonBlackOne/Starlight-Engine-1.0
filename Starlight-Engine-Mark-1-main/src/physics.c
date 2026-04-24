// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "physics.h"
#include <math.h>

static const vec3 GRAVITY = {0.0f, -9.81f, 0.0f};

void physics_rigidbody_init(RigidBody* rb, float mass, bool is_static) {
    glm_vec3_zero(rb->position);
    glm_vec3_zero(rb->velocity);
    glm_vec3_zero(rb->acceleration);
    glm_vec3_zero(rb->force_accumulator);
    rb->mass = mass;
    rb->inverse_mass = is_static ? 0.0f : (1.0f / mass);
    rb->restitution = 0.5f;
    rb->friction = 0.3f;
    rb->is_static = is_static;
    rb->use_gravity = !is_static;
}

void physics_apply_force(RigidBody* rb, vec3 force) {
    glm_vec3_add(rb->force_accumulator, force, rb->force_accumulator);
}

void physics_integrate(RigidBody* rb, float dt) {
    if (rb->is_static || rb->inverse_mass == 0.0f) return;

    // Gravidade
    if (rb->use_gravity) {
        vec3 gravity_force;
        glm_vec3_scale((float*)GRAVITY, rb->mass, gravity_force);
        glm_vec3_add(rb->force_accumulator, gravity_force, rb->force_accumulator);
    }

    // F = ma -> a = F/m
    glm_vec3_scale(rb->force_accumulator, rb->inverse_mass, rb->acceleration);

    // Semi-implicit Euler
    vec3 dv;
    glm_vec3_scale(rb->acceleration, dt, dv);
    glm_vec3_add(rb->velocity, dv, rb->velocity);

    vec3 dp;
    glm_vec3_scale(rb->velocity, dt, dp);
    glm_vec3_add(rb->position, dp, rb->position);

    // Damping leve (simula arrasto do ar)
    glm_vec3_scale(rb->velocity, 0.999f, rb->velocity);

    // Reset forcas acumuladas
    glm_vec3_zero(rb->force_accumulator);
}

ContactInfo physics_sphere_vs_plane(vec3 sphere_pos, float radius, vec3 plane_normal, float plane_dist) {
    ContactInfo info = {0};
    float dist = glm_vec3_dot(sphere_pos, plane_normal) - plane_dist;
    
    if (dist < radius) {
        info.has_collision = true;
        info.penetration = radius - dist;
        glm_vec3_copy(plane_normal, info.normal);
        
        vec3 offset;
        glm_vec3_scale(plane_normal, dist, offset);
        glm_vec3_sub(sphere_pos, offset, info.point);
    }
    return info;
}

ContactInfo physics_sphere_vs_sphere(vec3 pos_a, float radius_a, vec3 pos_b, float radius_b) {
    ContactInfo info = {0};
    vec3 diff;
    glm_vec3_sub(pos_a, pos_b, diff);
    float dist_sq = glm_vec3_norm2(diff);
    float radius_sum = radius_a + radius_b;
    
    if (dist_sq < radius_sum * radius_sum) {
        float dist = sqrtf(dist_sq);
        info.has_collision = true;
        info.penetration = radius_sum - dist;
        
        if (dist > 0.0001f) {
            glm_vec3_divs(diff, dist, info.normal);
        } else {
            info.normal[0] = 0.0f; info.normal[1] = 1.0f; info.normal[2] = 0.0f;
        }
        
        // Ponto de contato no meio
        vec3 half;
        glm_vec3_add(pos_a, pos_b, half);
        glm_vec3_scale(half, 0.5f, info.point);
    }
    return info;
}

ContactInfo physics_aabb_vs_aabb(vec3 pos_a, vec3 half_a, vec3 pos_b, vec3 half_b) {
    ContactInfo info = {0};
    vec3 diff;
    glm_vec3_sub(pos_a, pos_b, diff);
    
    float overlap_x = (half_a[0] + half_b[0]) - fabsf(diff[0]);
    float overlap_y = (half_a[1] + half_b[1]) - fabsf(diff[1]);
    float overlap_z = (half_a[2] + half_b[2]) - fabsf(diff[2]);
    
    if (overlap_x > 0 && overlap_y > 0 && overlap_z > 0) {
        info.has_collision = true;
        
        // Menor overlap = eixo de separacao
        if (overlap_x <= overlap_y && overlap_x <= overlap_z) {
            info.penetration = overlap_x;
            info.normal[0] = (diff[0] > 0) ? 1.0f : -1.0f;
            info.normal[1] = 0.0f; info.normal[2] = 0.0f;
        } else if (overlap_y <= overlap_x && overlap_y <= overlap_z) {
            info.penetration = overlap_y;
            info.normal[0] = 0.0f;
            info.normal[1] = (diff[1] > 0) ? 1.0f : -1.0f;
            info.normal[2] = 0.0f;
        } else {
            info.penetration = overlap_z;
            info.normal[0] = 0.0f; info.normal[1] = 0.0f;
            info.normal[2] = (diff[2] > 0) ? 1.0f : -1.0f;
        }
        
        glm_vec3_add(pos_a, pos_b, info.point);
        glm_vec3_scale(info.point, 0.5f, info.point);
    }
    return info;
}

float physics_swept_aabb(vec3 pos_a, vec3 half_a, vec3 vel_a, vec3 pos_b, vec3 half_b, vec3 vel_b, vec3 out_normal) {
    vec3 rel_vel;
    glm_vec3_sub(vel_a, vel_b, rel_vel);

    vec3 inv_entry, inv_exit;

    for (int i = 0; i < 3; i++) {
        if (rel_vel[i] > 0.0f) {
            inv_entry[i] = (pos_b[i] - half_b[i]) - (pos_a[i] + half_a[i]);
            inv_exit[i]  = (pos_b[i] + half_b[i]) - (pos_a[i] - half_a[i]);
        } else {
            inv_entry[i] = (pos_b[i] + half_b[i]) - (pos_a[i] - half_a[i]);
            inv_exit[i]  = (pos_b[i] - half_b[i]) - (pos_a[i] + half_a[i]);
        }
    }

    vec3 entry, exit;
    for (int i = 0; i < 3; i++) {
        if (rel_vel[i] == 0.0f) {
            entry[i] = -INFINITY;
            exit[i] = INFINITY;
        } else {
            entry[i] = inv_entry[i] / rel_vel[i];
            exit[i] = inv_exit[i] / rel_vel[i];
        }
    }

    float entry_time = fmaxf(fmaxf(entry[0], entry[1]), entry[2]);
    float exit_time = fminf(fminf(exit[0], exit[1]), exit[2]);

    // Check for no collision
    if (entry_time > exit_time || entry[0] < 0.0f && entry[1] < 0.0f && entry[2] < 0.0f || entry_time > 1.0f) {
        glm_vec3_zero(out_normal);
        return 1.0f; // Sem colisão no inter-frame
    } else {
        if (entry[0] > entry[1] && entry[0] > entry[2]) {
            out_normal[0] = rel_vel[0] < 0.0f ? 1.0f : -1.0f;
            out_normal[1] = 0.0f; out_normal[2] = 0.0f;
        } else if (entry[1] > entry[0] && entry[1] > entry[2]) {
            out_normal[0] = 0.0f;
            out_normal[1] = rel_vel[1] < 0.0f ? 1.0f : -1.0f;
            out_normal[2] = 0.0f;
        } else {
            out_normal[0] = 0.0f; out_normal[1] = 0.0f;
            out_normal[2] = rel_vel[2] < 0.0f ? 1.0f : -1.0f;
        }
        return entry_time;
    }
}

bool physics_ray_vs_aabb(vec3 ray_origin, vec3 ray_dir, vec3 aabb_min, vec3 aabb_max, float* hit_dist) {
    vec3 dir_safe;
    dir_safe[0] = ray_dir[0] == 0.0f ? 0.00001f : ray_dir[0];
    dir_safe[1] = ray_dir[1] == 0.0f ? 0.00001f : ray_dir[1];
    dir_safe[2] = ray_dir[2] == 0.0f ? 0.00001f : ray_dir[2];

    float tmin = (aabb_min[0] - ray_origin[0]) / dir_safe[0];
    float tmax = (aabb_max[0] - ray_origin[0]) / dir_safe[0];
    if (tmin > tmax) { float temp = tmin; tmin = tmax; tmax = temp; }

    float tymin = (aabb_min[1] - ray_origin[1]) / dir_safe[1];
    float tymax = (aabb_max[1] - ray_origin[1]) / dir_safe[1];
    if (tymin > tymax) { float temp = tymin; tymin = tymax; tymax = temp; }

    if ((tmin > tymax) || (tymin > tmax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (aabb_min[2] - ray_origin[2]) / dir_safe[2];
    float tzmax = (aabb_max[2] - ray_origin[2]) / dir_safe[2];
    if (tzmin > tzmax) { float temp = tzmin; tzmin = tzmax; tzmax = temp; }

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    if (tmin < 0.0f) {
        if (tmax < 0.0f) return false;
        if (hit_dist) *hit_dist = tmax;
        return true;
    }
    
    if (hit_dist) *hit_dist = tmin;
    return true;
}

void physics_resolve_collision(RigidBody* a, RigidBody* b, ContactInfo* contact) {
    if (!contact->has_collision) return;
    
    float inv_mass_sum = a->inverse_mass + b->inverse_mass;
    if (inv_mass_sum == 0.0f) return; // Dois estaticos
    
    // Separar corpos (positional correction)
    vec3 correction;
    float correction_amount = contact->penetration / inv_mass_sum * 0.8f;
    glm_vec3_scale(contact->normal, correction_amount, correction);
    
    vec3 ca, cb;
    glm_vec3_scale(correction, a->inverse_mass, ca);
    glm_vec3_add(a->position, ca, a->position);
    
    glm_vec3_scale(correction, -b->inverse_mass, cb);
    glm_vec3_add(b->position, cb, b->position);
    
    // Velocidade relativa
    vec3 rel_vel;
    glm_vec3_sub(a->velocity, b->velocity, rel_vel);
    float vel_along_normal = glm_vec3_dot(rel_vel, contact->normal);
    
    if (vel_along_normal > 0) return; // Ja se separando
    
    // Impulso
    float e = fminf(a->restitution, b->restitution);
    float j = -(1.0f + e) * vel_along_normal / inv_mass_sum;
    
    vec3 impulse;
    glm_vec3_scale(contact->normal, j, impulse);
    
    vec3 imp_a, imp_b;
    glm_vec3_scale(impulse, a->inverse_mass, imp_a);
    glm_vec3_add(a->velocity, imp_a, a->velocity);
    
    glm_vec3_scale(impulse, -b->inverse_mass, imp_b);
    glm_vec3_add(b->velocity, imp_b, b->velocity);
}

// --- Kinematic Character Controller (KCC) ---

void physics_kcc_init(KCC* kcc, vec3 start_pos, float radius, float height) {
    glm_vec3_copy(start_pos, kcc->position);
    glm_vec3_zero(kcc->velocity);
    kcc->radius = radius;
    kcc->height = height;
    kcc->step_height = 0.3f;
    kcc->max_slope = glm_rad(45.0f);
    kcc->is_grounded = false;
}

static ContactInfo kcc_vs_aabb(vec3 kcc_pos, float radius, float height, vec3 aabb_pos, vec3 aabb_half) {
    vec3 center = {kcc_pos[0], kcc_pos[1] + height * 0.5f, kcc_pos[2]};
    vec3 closest;
    for (int i=0; i<3; ++i) {
        closest[i] = glm_clamp(center[i], aabb_pos[i] - aabb_half[i], aabb_pos[i] + aabb_half[i]);
    }
    vec3 diff;
    glm_vec3_sub(center, closest, diff);
    float distSq = glm_vec3_norm2(diff);
    
    ContactInfo info = {0};
    if (distSq < radius * radius) {
        float dist = sqrtf(distSq);
        info.has_collision = true;
        info.penetration = radius - dist;
        if (dist > 0.0001f) {
            glm_vec3_scale(diff, 1.0f / dist, info.normal);
        } else {
            info.normal[0] = 0; info.normal[1] = 1; info.normal[2] = 0; 
        }
    }
    return info;
}

void physics_kcc_move(
    KCC* kcc, 
    vec3 movement, 
    PlaneCollider* planes, int num_planes,
    SphereCollider* spheres, vec3* sphere_positions, int num_spheres,
    AABBCollider* aabbs, vec3* aabb_positions, int num_aabbs
) {
    kcc->is_grounded = false;
    vec3 target_pos;
    glm_vec3_add(kcc->position, movement, target_pos);

    int max_iterations = 4;
    for (int iter = 0; iter < max_iterations; ++iter) {
        bool collided = false;
        vec3 total_depenetration = GLM_VEC3_ZERO_INIT;
        int hit_count = 0;
        
        vec3 center = {target_pos[0], target_pos[1] + kcc->height * 0.5f, target_pos[2]};

        for (int i=0; i<num_planes; ++i) {
            ContactInfo hit = physics_sphere_vs_plane(center, kcc->radius, planes[i].normal, planes[i].distance);
            if (hit.has_collision) {
                if (hit.normal[1] > cosf(kcc->max_slope)) kcc->is_grounded = true;
                vec3 push; glm_vec3_scale(hit.normal, hit.penetration, push);
                glm_vec3_add(total_depenetration, push, total_depenetration);
                collided = true; hit_count++;
            }
        }
        
        for (int i=0; i<num_aabbs; ++i) {
            ContactInfo hit = kcc_vs_aabb(target_pos, kcc->radius, kcc->height, aabb_positions[i], aabbs[i].half_extents);
            if (hit.has_collision) {
                if (hit.normal[1] > cosf(kcc->max_slope)) kcc->is_grounded = true;
                vec3 push; glm_vec3_scale(hit.normal, hit.penetration, push);
                glm_vec3_add(total_depenetration, push, total_depenetration);
                collided = true; hit_count++;
            }
        }

        if (collided && hit_count > 0) {
            glm_vec3_scale(total_depenetration, 1.0f / (float)hit_count, total_depenetration);
            glm_vec3_add(target_pos, total_depenetration, target_pos);
        } else {
            break;
        }
    }
    
    glm_vec3_copy(target_pos, kcc->position);
}
