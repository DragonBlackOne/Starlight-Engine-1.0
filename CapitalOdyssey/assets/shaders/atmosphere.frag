#version 330 core

// Atmospheric Scattering Shader
// Implements physically-based sky rendering using Rayleigh and Mie scattering

out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 u_camera_pos;
uniform mat4 u_inv_proj;
uniform mat4 u_inv_view;
uniform vec3 u_sun_dir;
uniform float u_sun_intensity;

// Atmosphere parameters
const vec3 BETA_R = vec3(5.8e-6, 13.5e-6, 33.1e-6); // Rayleigh scattering coefficient
const vec3 BETA_M = vec3(21e-6);                     // Mie scattering coefficient
const float EARTH_RADIUS = 6371e3;                   // Earth radius in meters
const float ATMOSPHERE_RADIUS = 6471e3;              // Atmosphere radius
const float H_R = 7994.0;                            // Rayleigh scale height
const float H_M = 1200.0;                            // Mie scale height
const float G = 0.76;                                // Mie phase anisotropy

// Rayleigh phase function
float rayleigh_phase(float cos_theta) {
    return 3.0 / (16.0 * 3.14159) * (1.0 + cos_theta * cos_theta);
}

// Henyey-Greenstein phase function (Mie)
float mie_phase(float cos_theta, float g) {
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.14159 * pow(1.0 + g2 - 2.0 * g * cos_theta, 1.5));
}

// Ray-sphere intersection
bool ray_sphere_intersect(vec3 ro, vec3 rd, float radius, out float t0, out float t1) {
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, ro);
    float c = dot(ro, ro) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;
    
    if (discriminant < 0.0) {
        return false;
    }
    
    float sqrt_disc = sqrt(discriminant);
    t0 = (-b - sqrt_disc) / (2.0 * a);
    t1 = (-b + sqrt_disc) / (2.0 * a);
    
    return true;
}

vec3 compute_scattering(vec3 ray_origin, vec3 ray_dir, vec3 sun_dir) {
    const int SAMPLES = 16;
    const int LIGHT_SAMPLES = 8;
    
    // Adjust ray origin to planet center
    ray_origin.y += EARTH_RADIUS;
    
    float t0, t1;
    if (!ray_sphere_intersect(ray_origin, ray_dir, ATMOSPHERE_RADIUS, t0, t1)) {
        return vec3(0.0);
    }
    
    // Clamp to ground
    float ground_t0, ground_t1;
    bool hit_ground = ray_sphere_intersect(ray_origin, ray_dir, EARTH_RADIUS, ground_t0, ground_t1);
    
    if (hit_ground && ground_t0 > 0.0) {
        t1 = min(t1, ground_t0);
    }
    
    t0 = max(t0, 0.0);
    
    float step_size = (t1 - t0) / float(SAMPLES);
    float t = t0;
    
    vec3 total_rayleigh = vec3(0.0);
    vec3 total_mie = vec3(0.0);
    float optical_depth_r = 0.0;
    float optical_depth_m = 0.0;
    
    float cos_theta = dot(ray_dir, sun_dir);
    float phase_r = rayleigh_phase(cos_theta);
    float phase_m = mie_phase(cos_theta, G);
    
    for (int i = 0; i < SAMPLES; i++) {
        vec3 sample_pos = ray_origin + ray_dir * (t + step_size * 0.5);
        float height = length(sample_pos) - EARTH_RADIUS;
        
        // Density at this point
        float density_r = exp(-height / H_R) * step_size;
        float density_m = exp(-height / H_M) * step_size;
        
        optical_depth_r += density_r;
        optical_depth_m += density_m;
        
        // Sample light
        float light_t0, light_t1;
        ray_sphere_intersect(sample_pos, sun_dir, ATMOSPHERE_RADIUS, light_t0, light_t1);
        
        float light_step = light_t1 / float(LIGHT_SAMPLES);
        float light_t = 0.0;
        float optical_depth_light_r = 0.0;
        float optical_depth_light_m = 0.0;
        
        for (int j = 0; j < LIGHT_SAMPLES; j++) {
            vec3 light_sample_pos = sample_pos + sun_dir * (light_t + light_step * 0.5);
            float light_height = length(light_sample_pos) - EARTH_RADIUS;
            
            optical_depth_light_r += exp(-light_height / H_R) * light_step;
            optical_depth_light_m += exp(-light_height / H_M) * light_step;
            
            light_t += light_step;
        }
        
        // Attenuation
        vec3 attenuation = exp(-(BETA_R * (optical_depth_r + optical_depth_light_r) + 
                                  BETA_M * (optical_depth_m + optical_depth_light_m)));
        
        total_rayleigh += density_r * attenuation;
        total_mie += density_m * attenuation;
        
        t += step_size;
    }
    
    vec3 rayleigh = BETA_R * phase_r * total_rayleigh;
    vec3 mie = BETA_M * phase_m * total_mie;
    
    return (rayleigh + mie) * u_sun_intensity * 20.0;
}

void main() {
    // Reconstruct world direction
    vec4 ndc = vec4(TexCoords * 2.0 - 1.0, 1.0, 1.0);
    vec4 view_pos = u_inv_proj * ndc;
    view_pos /= view_pos.w;
    
    vec3 world_dir = normalize((u_inv_view * vec4(view_pos.xyz, 0.0)).xyz);
    
    // Compute atmospheric scattering
    vec3 color = compute_scattering(u_camera_pos, world_dir, u_sun_dir);
    
    // Tone mapping
    color = 1.0 - exp(-color);
    
    FragColor = vec4(color, 1.0);
}
