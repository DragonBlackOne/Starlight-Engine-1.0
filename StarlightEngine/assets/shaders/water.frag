#version 330 core

in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_tex_ids;
in vec3 v_world_pos;
in float v_ao;

out vec4 f_color;

uniform sampler2D u_texture;
uniform vec3 u_light_dir;
uniform vec3 u_view_pos;
uniform float u_time;

// Atlas settings
const float ATLAS_ROWS = 4.0;
const float ATLAS_COLS = 4.0;

// Simple Noise for Normal Perturbation
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

void main() {
    float tex_id = v_tex_ids.x;
    int index = int(tex_id);
    float col = float(index % 4);
    float row = float(index / 4);
    
    vec2 uv = v_texcoord / vec2(ATLAS_COLS, ATLAS_ROWS);
    uv += vec2(col / ATLAS_COLS, row / ATLAS_ROWS);
    
    vec4 tex_color = texture(u_texture, uv);
    
    // Advanced Normal Mapping (Noise-based waves)
    vec3 base_normal = normalize(v_normal);
    float n1 = noise(v_world_pos.xz * 0.5 + u_time * 0.5);
    float n2 = noise(v_world_pos.xz * 1.0 - u_time * 0.8);
    vec3 wave_normal = normalize(base_normal + vec3(n1 - 0.5, 0.0, n2 - 0.5) * 0.2);
    
    vec3 light_dir = normalize(u_light_dir);
    vec3 view_dir = normalize(u_view_pos - v_world_pos);
    
    // PBR Lighting (Simplified Cook-Torrance)
    float diff = max(dot(wave_normal, light_dir), 0.0);
    vec3 half_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(wave_normal, half_dir), 0.0), 128.0);
    
    // Fresnel
    float F0 = 0.02; // Water reflectance at normal incidence
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(view_dir, wave_normal), 0.0), 5.0);
    
    // Shading
    vec3 water_color = tex_color.rgb * (0.4 + diff * 0.6);
    vec3 sky_color = vec3(0.4, 0.7, 1.0); // Slightly more vibrant sky
    
    vec3 final_rgb = mix(water_color, sky_color, fresnel * 0.7) + spec * 1.5;
    
    // Underwater depth/fog effect
    float dist = length(u_view_pos - v_world_pos);
    float fog_factor = clamp((dist - 10.0) / 70.0, 0.0, 1.0);
    final_rgb = mix(final_rgb, sky_color * 0.5, fog_factor);
    
    f_color = vec4(final_rgb * v_ao, 0.85); // Slightly more opaque
}
