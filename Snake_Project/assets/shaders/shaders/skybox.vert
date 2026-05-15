#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 v_uv;

uniform mat4 projection;
uniform mat4 view;

void main() {
    v_uv = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Force depth to 1.0
}
