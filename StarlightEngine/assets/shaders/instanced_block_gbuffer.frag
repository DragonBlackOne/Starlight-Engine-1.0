#version 330 core

// Instanced Block Shader - G-Buffer Pass

in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_tex_ids; // (Top, Side, Bottom)
in vec3 v_world_pos;
in float v_ao;     // Brightness factor from AO

// G-Buffer Outputs
layout(location = 0) out vec4 g_albedo;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec3 g_position;

uniform sampler2D u_texture;

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
    
    // Apply Vertex AO to Albedo directly
    // This bakes the static AO into the color, which is fine for voxel games
    vec3 final_albedo = tex_color.rgb * v_ao;
    
    // Output to G-Buffer
    g_albedo = vec4(final_albedo, 1.0);
    g_normal = normal;
    g_position = v_world_pos;
}
