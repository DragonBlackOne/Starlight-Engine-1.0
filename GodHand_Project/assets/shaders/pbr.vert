#version 410 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in ivec4 in_bone_ids;
layout (location = 4) in vec4 in_weights;

out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;
out float v_clip_depth;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
uniform mat4 bone_matrices[MAX_BONES];
uniform bool has_anim;

void main() {
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);

    if (has_anim) {
        for(int i = 0 ; i < 4 ; i++) {
            if(in_bone_ids[i] == -1) continue;
            if(in_bone_ids[i] >= MAX_BONES) break;

            vec4 localPosition = bone_matrices[in_bone_ids[i]] * vec4(in_position, 1.0);
            totalPosition += localPosition * in_weights[i];

            vec3 localNormal = mat3(bone_matrices[in_bone_ids[i]]) * in_normal;
            totalNormal += localNormal * in_weights[i];
        }
    } else {
        totalPosition = vec4(in_position, 1.0);
        totalNormal = in_normal;
    }

    vec4 world_pos = model * totalPosition;
    WorldPos = world_pos.xyz;
    Normal = mat3(transpose(inverse(model))) * totalNormal;
    TexCoords = in_uv;

    vec4 viewPos = view * world_pos;
    v_clip_depth = -viewPos.z;

    gl_Position = projection * viewPos;
}
