#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIds;
layout (location = 4) in vec4 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 bone_matrices[MAX_BONES];
uniform bool has_anim;

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);

    if (has_anim) {
        for(int i = 0 ; i < 4 ; i++) {
            if(aBoneIds[i] == -1) continue;
            if(aBoneIds[i] >= MAX_BONES) break;

            vec4 localPosition = bone_matrices[aBoneIds[i]] * vec4(aPos, 1.0);
            totalPosition += localPosition * aWeights[i];

            vec3 localNormal = mat3(bone_matrices[aBoneIds[i]]) * aNormal;
            totalNormal += localNormal * aWeights[i];
        }
    } else {
        totalPosition = vec4(aPos, 1.0);
        totalNormal = aNormal;
    }

    vec4 worldPos = model * totalPosition;
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * totalNormal;

    gl_Position = projection * view * worldPos;
}
