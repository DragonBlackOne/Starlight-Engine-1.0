#version 430 core

uniform vec4 u_id_color;
out vec4 f_color;

void main() {
    f_color = u_id_color;
}
