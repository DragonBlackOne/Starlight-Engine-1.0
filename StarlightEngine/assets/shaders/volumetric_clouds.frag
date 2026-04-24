#version 330 core

// Volumetric Clouds Shader (Raymarching)
// Implements realistic volumetric clouds using noise-based density

out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 u_camera_pos;
uniform mat4 u_inv_proj;
uniform mat4 u_inv_view;
uniform float u_time;
uniform vec3 u_sun_dir;

// Cloud parameters
uniform float u_cloud_coverage;  // 0.0 - 1.0
uniform float u_cloud_density;   // Cloud thickness
uniform float u_cloud_height_min;
uniform float u_cloud_height_max;

// 3D Noise function (Perlin-like)
float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    
    return mix(mix(mix(hash(i + vec3(0,0,0)), 
                       hash(i + vec3(1,0,0)), f.x),
                   mix(hash(i + vec3(0,1,0)), 
                       hash(i + vec3(1,1,0)), f.x), f.y),
               mix(mix(hash(i + vec3(0,0,1)), 
                       hash(i + vec3(1,0,1)), f.x),
                   mix(hash(i + vec3(0,1,1)), 
                       hash(i + vec3(1,1,1)), f.x), f.y), f.z);
}

// Fractal Brownian Motion
float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    
    return value;
}

// Cloud density function
float cloud_density(vec3 pos) {
    // Check if in cloud layer
    if (pos.y < u_cloud_height_min || pos.y > u_cloud_height_max) {
        return 0.0;
    }
    
    // Animate clouds
    vec3 wind = vec3(u_time * 0.02, 0.0, u_time * 0.01);
    vec3 sample_pos = pos * 0.002 + wind;
    
    // Base cloud shape
    float base_cloud = fbm(sample_pos);
    
    // Apply coverage
    base_cloud = smoothstep(1.0 - u_cloud_coverage, 1.0, base_cloud);
    
    // Height gradient (more dense at bottom)
    float height_fraction = (pos.y - u_cloud_height_min) / (u_cloud_height_max - u_cloud_height_min);
    float height_gradient = smoothstep(0.0, 0.3, height_fraction) * smoothstep(1.0, 0.7, height_fraction);
    
    return base_cloud * height_gradient * u_cloud_density;
}

// Henyey-Greenstein phase function
float hg_phase(float cos_theta, float g) {
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.14159 * pow(1.0 + g2 - 2.0 * g * cos_theta, 1.5));
}

vec4 raymarch_clouds(vec3 ray_origin, vec3 ray_dir) {
    const int MAX_STEPS = 64;
    const float STEP_SIZE = 50.0;
    
    vec3 pos = ray_origin;
    float transmittance = 1.0;
    vec3 scattered_light = vec3(0.0);
    
    // Find entry point into cloud layer
    if (ray_origin.y < u_cloud_height_min) {
        float t = (u_cloud_height_min - ray_origin.y) / ray_dir.y;
        if (t > 0.0) {
            pos = ray_origin + ray_dir * t;
        }
    }
    
    for (int i = 0; i < MAX_STEPS; i++) {
        // Check if outside cloud layer
        if (pos.y < u_cloud_height_min || pos.y > u_cloud_height_max) {
            break;
        }
        
        float density = cloud_density(pos);
        
        if (density > 0.01) {
            // Calculate light contribution
            float cos_theta = dot(ray_dir, u_sun_dir);
            float phase = hg_phase(cos_theta, 0.3);
            
            // Simple in-scattering
            vec3 sun_color = vec3(1.0, 0.95, 0.8);
            float light_energy = phase * density;
            
            scattered_light += transmittance * sun_color * light_energy * STEP_SIZE * 0.01;
            
            // Absorption
            transmittance *= exp(-density * STEP_SIZE * 0.01);
            
            if (transmittance < 0.01) {
                break;
            }
        }
        
        pos += ray_dir * STEP_SIZE;
    }
    
    return vec4(scattered_light, 1.0 - transmittance);
}

void main() {
    // Reconstruct world position
    vec4 ndc = vec4(TexCoords * 2.0 - 1.0, 1.0, 1.0);
    vec4 view_pos = u_inv_proj * ndc;
    view_pos /= view_pos.w;
    
    vec3 world_dir = normalize((u_inv_view * vec4(view_pos.xyz, 0.0)).xyz);
    
    // Only render clouds for upward-facing rays
    if (world_dir.y > 0.0) {
        vec4 cloud = raymarch_clouds(u_camera_pos, world_dir);
        FragColor = cloud;
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
