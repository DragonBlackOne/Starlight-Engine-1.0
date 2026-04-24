#version 410 core

layout (location = 0) in vec3 in_position;
layout (location = 3) in vec4 in_joints;
layout (location = 4) in vec4 in_weights;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

// Skinning support
uniform mat4 u_joint_mat[64];
uniform bool u_use_skinning;

void main() {
    mat4 model_mat = u_model;
    
    if (u_use_skinning) {
        mat4 skin_mat = 
            in_weights.x * u_joint_mat[int(in_joints.x)] +
            in_weights.y * u_joint_mat[int(in_joints.y)] +
            in_weights.z * u_joint_mat[int(in_joints.z)] +
            in_weights.w * u_joint_mat[int(in_joints.w)];
        model_mat = u_model * skin_mat;
    }

    gl_Position = u_projection * u_view * model_mat * vec4(in_position, 1.0);
}
