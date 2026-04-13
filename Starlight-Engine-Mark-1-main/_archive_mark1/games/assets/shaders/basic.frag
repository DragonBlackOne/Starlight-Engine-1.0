#version 330 core
in vec2 v_uv;
out vec4 f_color;
uniform sampler2D u_texture;
uniform vec4 u_color;
uniform bool u_use_texture;
void main() {
    vec4 base = u_color;
    if (u_use_texture) {
        base *= texture(u_texture, v_uv);
    }
    f_color = base;
}
