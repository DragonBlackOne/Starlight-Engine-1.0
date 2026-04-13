#version 410 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec4 in_joints;
layout (location = 5) in vec4 in_weights;
layout (location = 6) in mat4 in_instance_mat;

out vec3 v_normal;
out vec3 v_frag_pos;
out vec2 v_uv;
out mat3 v_tbn;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform mat4 u_joint_mat[64]; // Joint Palette
uniform bool u_use_skinning;
uniform bool u_is_instanced;
uniform bool u_is_boids;

struct Boid {
    vec4 position;
    vec4 velocity;
};



uniform float u_log_c = 1.0;
uniform float u_log_far = 10000.0;

void main() {
    mat4 model_mat;
    if (u_use_skinning) {
        mat4 skin_mat = 
            in_weights.x * u_joint_mat[int(in_joints.x)] +
            in_weights.y * u_joint_mat[int(in_joints.y)] +
            in_weights.z * u_joint_mat[int(in_joints.z)] +
            in_weights.w * u_joint_mat[int(in_joints.w)];
        model_mat = u_model * skin_mat;
    } else  if (u_is_instanced) {
        model_mat = in_instance_mat;
    } else {
        model_mat = u_model;
    }

    vec4 world_pos = model_mat * vec4(in_position, 1.0);
    mat3 normal_mat = transpose(inverse(mat3(model_mat)));

    gl_Position = u_projection * u_view * world_pos;

    v_frag_pos = world_pos.xyz;
    v_normal = (length(in_normal) > 0.1) ? normalize(normal_mat * in_normal) : normalize(normal_mat * vec3(0.0, 1.0, 0.0));
    v_uv = in_uv;

    // TBN Matrix with NaN Safety
    vec3 T = (length(in_tangent) > 0.1) ? normalize(normal_mat * in_tangent) : normalize(normal_mat * vec3(1.0, 0.0, 0.0));
    vec3 N = v_normal;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    v_tbn = mat3(T, B, N);
}
