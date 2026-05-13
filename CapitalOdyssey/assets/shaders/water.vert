#version 330 core

in vec3 in_position;
in vec3 in_normal;
in vec2 in_texcoord;

// Instance attributes
in vec3 in_instance_pos;    // Block position
in vec3 in_instance_tex_ids; // Texture IDs
in float in_instance_ao;    // Ambient Occlusion

uniform mat4 m_view;
uniform mat4 m_proj;
uniform float u_time;

out vec3 v_normal;
out vec2 v_texcoord;
out vec3 v_tex_ids;
out vec3 v_world_pos;
out float v_ao;

void main() {
    // Apply instance transform
    vec3 world_pos = in_position + in_instance_pos;
    
    // Water displacement (Waves)
    // Only displace top vertices (y > 0.4)
    if (in_position.y > 0.4) {
        float wave = sin(world_pos.x * 2.0 + u_time * 2.0) * 0.05 + 
                     cos(world_pos.z * 1.5 + u_time * 1.5) * 0.05;
        world_pos.y += wave - 0.1; // Lower water slightly
    }
    
    v_world_pos = world_pos;
    
    gl_Position = m_proj * m_view * vec4(world_pos, 1.0);
    
    v_normal = in_normal;
    v_texcoord = in_texcoord;
    v_tex_ids = in_instance_tex_ids;
    v_ao = in_instance_ao;
}
