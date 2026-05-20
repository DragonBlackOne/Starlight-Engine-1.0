#version 330

in vec3 in_position; // Not used for point sprites usually, but good for instancing

// Particle data from SSBO (if using instancing) or Vertex Buffer
// For this implementation, we'll assume we read from the same buffer as vertex attributes
layout(location = 0) in vec4 in_particle_pos; // xyz = pos, w = life
layout(location = 1) in vec4 in_particle_vel; // xyz = vel, w = size
layout(location = 2) in vec4 in_particle_color;

uniform mat4 m_view;
uniform mat4 m_proj;

out vec4 v_color;
out float v_life;

void main() {
    // Position
    vec3 pos = in_particle_pos.xyz;
    float size = in_particle_vel.w;
    
    gl_Position = m_proj * m_view * vec4(pos, 1.0);
    if (in_particle_pos.w <= 0.0) {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0); // Out of clip space
    }
    
    // Point size attenuation
    float dist = length((m_view * vec4(pos, 1.0)).xyz);
    gl_PointSize = size * (1000.0 / dist); // Scale by distance
    
    v_color = in_particle_color;
    v_life = in_particle_pos.w;
}
