#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) in vec4 in_joints;
layout (location = 4) in vec4 in_weights;

uniform mat4 u_model;
uniform mat4 u_light_matrix;

// Skinning
uniform bool u_is_skinned;
uniform mat4 u_jointMatrices[64];

void main() {
    vec4 local_pos;
    if (u_is_skinned) {
        mat4 skinMatrix = 
            in_weights.x * u_jointMatrices[int(in_joints.x)] +
            in_weights.y * u_jointMatrices[int(in_joints.y)] +
            in_weights.z * u_jointMatrices[int(in_joints.z)] +
            in_weights.w * u_jointMatrices[int(in_joints.w)];

        local_pos = skinMatrix * vec4(in_position, 1.0);
    } else {
        local_pos = vec4(in_position, 1.0);
    }

    gl_Position = u_light_matrix * u_model * local_pos;
}