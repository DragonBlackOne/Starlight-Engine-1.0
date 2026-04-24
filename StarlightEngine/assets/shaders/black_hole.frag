
#version 330 core

out vec4 FragColor;
in vec2 v_uv;

uniform vec3 u_view_pos;
uniform mat4 u_inv_proj;
uniform mat4 u_inv_view;
uniform vec3 u_bh_pos;
uniform float u_bh_radius;
uniform float u_time;

// Simple star background noise
float hash(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }

void main() {
    // 1. Ray setup
    vec4 clip_pos = vec4(v_uv * 2.0 - 1.0, -1.0, 1.0);
    vec4 view_pos = u_inv_proj * clip_pos;
    view_pos /= view_pos.w;
    vec4 world_pos = u_inv_view * view_pos;
    vec3 rd = normalize(world_pos.xyz - u_view_pos);
    vec3 ro = u_view_pos;
    
    // 2. Gravitational Lensing Ray-Marching
    vec3 p = ro;
    vec3 v = rd;
    float dt = 1.0;
    bool event_horizon = false;
    
    for(int i = 0; i < 60; i++) {
        vec3 diff = u_bh_pos - p;
        float dist_sq = dot(diff, diff);
        float dist = sqrt(dist_sq);
        
        if(dist < u_bh_radius) {
            event_horizon = true;
            break;
        }
        
        // Gravitational force bends the velocity vector
        // a = G*M / r^2 -> for light we use a simplified bending
        float bending = (u_bh_radius * 2.0) / dist_sq;
        v = normalize(v + normalize(diff) * bending * dt);
        
        p += v * dt * max(dist * 0.1, 1.0);
        if (length(p - ro) > 5000.0) break;
    }
    
    // 3. Shading
    if(event_horizon) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        // Sample distant stars (background)
        vec3 star_dir = v;
        float stars = pow(hash(star_dir.xy * 100.0 + star_dir.z * 50.0), 20.0);
        
        // Accretion Disk (fake volumetric)
        vec3 bh_diff = p - u_bh_pos;
        float ring = abs(length(bh_diff.xz) - u_bh_radius * 3.5);
        float glow = exp(-ring * 0.5) * exp(-abs(bh_diff.y) * 2.0);
        
        vec3 final_col = vec3(stars) + vec3(1.0, 0.6, 0.2) * glow * 2.0;
        FragColor = vec4(final_col, 1.0);
    }
}
