#version 330 core

in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_tex_ids; // (Top, Side, Bottom)
in vec3 v_world_pos;
in float v_ao;     // Brightness factor from AO

out vec4 f_color;

uniform sampler2D u_texture;
uniform vec3 u_light_dir;
uniform vec3 u_view_pos;

// Atlas settings
const float ATLAS_ROWS = 4.0;
const float ATLAS_COLS = 4.0;

void main() {
    // Select texture ID based on normal
    float tex_id;
    vec3 normal = normalize(v_normal);
    
    if (normal.y > 0.5) {
        tex_id = v_tex_ids.x; // Top
    } else if (normal.y < -0.5) {
        tex_id = v_tex_ids.z; // Bottom
    } else {
        tex_id = v_tex_ids.y; // Side
    }
    
    // Calculate UVs for atlas
    int index = int(tex_id);
    float col = float(index % 4);
    float row = float(index / 4);
    
    // Scale UV to 1/4 size and offset
    vec2 uv = v_texcoord / vec2(ATLAS_COLS, ATLAS_ROWS);
    uv += vec2(col / ATLAS_COLS, row / ATLAS_ROWS);
    
    // Sample texture
    vec4 tex_color = texture(u_texture, uv);
    
    // Lighting
    vec3 light_dir = normalize(u_light_dir);
    
    // Diffuse (Warm Sunlight)
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 sun_color = vec3(1.0, 0.95, 0.8); // Warm sun
    vec3 diffuse = diff * sun_color;
    
    // Ambient (Cool Skylight)
    vec3 ambient_color = vec3(0.6, 0.7, 0.9); // Cool sky
    vec3 ambient = ambient_color * 0.6; // Increased ambient brightness
    
    // Specular (weak)
    vec3 view_dir = normalize(u_view_pos - v_world_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    float specular = 0.1 * spec; // Increased specular
    
    // Fog
    float dist = length(u_view_pos - v_world_pos);
    float fog_start = 30.0;
    float fog_end = 90.0; // Pushed back slightly
    float fog_factor = clamp((dist - fog_start) / (fog_end - fog_start), 0.0, 1.0);
    vec3 fog_color = vec3(0.53, 0.81, 0.92); // Sky blue
    
    // Combine
    vec3 lighting = (ambient + diffuse) * tex_color.rgb + specular;
    
    // Apply AO (multiply)
    lighting *= v_ao;
    
    // Mix Fog
    vec3 final_color = mix(lighting, fog_color, fog_factor);
    
    // --- POST PROCESSING (Simulated) ---
    
    // 1. Vibrance (Boost saturation of mid-tones)
    float luminance = dot(final_color, vec3(0.299, 0.587, 0.114));
    vec3 gray = vec3(luminance);
    vec3 saturated = mix(gray, final_color, 1.3); // 30% saturation boost
    final_color = mix(gray, saturated, 1.0);
    
    // 2. Tone Mapping (Reinhard)
    // Maps high dynamic range values to [0, 1]
    final_color = final_color / (final_color + vec3(1.0));
    
    // 3. Gamma Correction
    final_color = pow(final_color, vec3(1.0 / 2.2));
    
    f_color = vec4(final_color, 1.0);
}
