#version 330 core

in vec3 in_position;
in vec3 in_normal;
in vec3 in_color;

// Instance attributes
in vec3 in_instance_pos;    // Tree position
in float in_instance_scale; // Tree scale
in vec3 in_instance_color;  // Tree color variation

uniform mat4 m_view;
uniform mat4 m_proj;
uniform float u_time;
uniform float u_wind_strength;
uniform float u_wind_speed;

out vec3 v_color;
out vec3 v_normal;
out float v_wind_factor;

void main() {
    // Wind animation (affects top vertices more)
    float height_factor = in_position.y / 10.0; // Normalized height
    vec3 wind_offset = vec3(
        sin(u_time * u_wind_speed + in_instance_pos.x * 0.1) * u_wind_strength * height_factor,
        0.0,
        cos(u_time * u_wind_speed + in_instance_pos.z * 0.1) * u_wind_strength * height_factor
    );
    
    // Apply instance transform with wind
    vec3 world_pos = in_position * in_instance_scale + in_instance_pos + wind_offset;
    
    gl_Position = m_proj * m_view * vec4(world_pos, 1.0);
    
    // Pass varying data
    v_color = in_color * in_instance_color;
    v_normal = in_normal;
    v_wind_factor = height_factor;
}
