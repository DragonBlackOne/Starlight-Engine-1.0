#version 330

// Instanced Block Shader - G-Buffer Pass

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// Instance Data
layout(location = 3) in vec3 in_instance_pos;
layout(location = 4) in vec3 in_instance_tex_ids; // Top, Side, Bottom
layout(location = 5) in float in_instance_ao;

uniform mat4 m_proj;
uniform mat4 m_view;

out vec3 v_normal;
out vec3 v_world_pos;
out vec2 v_texcoord;
out vec3 v_tex_ids;
out float v_ao;

void main() {
    // Calculate world position
    vec3 world_pos = in_position + in_instance_pos;
    v_world_pos = world_pos;
    
    // Pass data to fragment
    v_normal = in_normal; // Cube normals are axis aligned, no rotation needed yet
    v_texcoord = in_texcoord;
    v_tex_ids = in_instance_tex_ids;
    v_ao = in_instance_ao;
    
    gl_Position = m_proj * m_view * vec4(world_pos, 1.0);
}
