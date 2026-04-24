#version 330 core
in vec3 in_position;
in vec3 in_normal;
in vec2 in_texcoord;
out vec3 v_normal;
out vec3 v_frag_pos;
out vec2 v_uv;
out vec4 v_frag_pos_light_space;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_light_space_matrix;
void main() {
    gl_Position = u_proj * u_view * u_model * vec4(in_position, 1.0);
    v_frag_pos = vec3(u_model * vec4(in_position, 1.0));
    v_normal = mat3(transpose(inverse(u_model))) * in_normal;
    v_uv = in_texcoord;
    v_frag_pos_light_space = u_light_space_matrix * vec4(v_frag_pos, 1.0);
}
