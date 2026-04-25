#version 410 core
layout (location = 0) out vec4 outColor;

in vec3 v_normal;
in vec3 v_frag_pos;
in vec2 v_uv;

uniform vec3 albedo;

void main() {
    // Just output the color directly to see if anything draws
    outColor = vec4(albedo, 1.0);
}
