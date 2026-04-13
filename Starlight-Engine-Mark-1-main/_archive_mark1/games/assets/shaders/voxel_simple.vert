#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 v_normal;
out vec3 v_position;
out vec2 v_uv;

uniform mat4 m_proj;
uniform mat4 m_view;
uniform mat4 m_model;

void main() {
    v_position = (m_model * vec4(in_position, 1.0)).xyz;
    v_normal = mat3(transpose(inverse(m_model))) * in_normal;
    v_uv = in_uv;
    gl_Position = m_proj * m_view * m_model * vec4(in_position, 1.0);
}
