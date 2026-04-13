#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in uvec4 aBoneIds;
layout (location = 3) in vec4 aBoneWeights;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_LightSpaceMatrix;

const int MAX_BONES = 120;
uniform mat4 u_BoneMatrices[MAX_BONES];
uniform bool u_HasAnimation;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    if (u_HasAnimation) {
        for(int i = 0 ; i < 4 ; i++) {
            if(aBoneIds[i] >= uint(MAX_BONES)) continue;
            vec4 localPosition = u_BoneMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
            totalPosition += localPosition * aBoneWeights[i];
        }
    } else {
        totalPosition = vec4(aPos, 1.0f);
    }

    TexCoords = aTexCoords;
    FragPos = vec3(model * totalPosition);
    Normal = mat3(transpose(inverse(model))) * vec3(0.0, 1.0, 0.0); 
    FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * model * totalPosition;
}
