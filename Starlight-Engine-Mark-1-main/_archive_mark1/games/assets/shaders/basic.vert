#version 330 core
in vec3 in_position;
in vec2 in_texcoord;
out vec2 v_uv;
uniform mat4 u_mvp;
void main() {
    gl_Position = u_mvp * vec4(in_position, 1.0);
    v_uv = in_texcoord;
}
