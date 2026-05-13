#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aJoints;
layout (location = 4) in vec4 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

// Skinning
uniform mat4 finalBonesMatrices[100];
uniform bool hasSkin;

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    
    if (hasSkin) {
        for(int i = 0 ; i < 4 ; i++)
        {
            if(aJoints[i] == -1) continue;
            if(aJoints[i] >= 100) break;
            
            vec4 localPosition = finalBonesMatrices[int(aJoints[i])] * vec4(aPos, 1.0);
            totalPosition += localPosition * aWeights[i];
            
            vec3 localNormal = mat3(finalBonesMatrices[int(aJoints[i])]) * aNormal;
            totalNormal += localNormal * aWeights[i];
        }
    } else {
        totalPosition = vec4(aPos, 1.0);
        totalNormal = aNormal;
    }

    FragPos = vec3(model * totalPosition);
    Normal = mat3(transpose(inverse(model))) * totalNormal;
    TexCoord = aTexCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
