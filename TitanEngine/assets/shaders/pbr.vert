// Este projeto é feito por IA e só o prompt é feito por um humano.
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aJoints;
layout (location = 4) in vec4 aWeights;

layout (location = 6) in mat4 aInstanceMatrix;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool uIsInstanced = false;
uniform bool uIsAnimated = false;
uniform mat4 uJointMatrices[64];

void main()
{
    mat4 modelMatrix = uIsInstanced ? aInstanceMatrix : model;
    
    if (uIsAnimated) {
        mat4 skinMatrix = 
            aWeights.x * uJointMatrices[aJoints.x] +
            aWeights.y * uJointMatrices[aJoints.y] +
            aWeights.z * uJointMatrices[aJoints.z] +
            aWeights.w * uJointMatrices[aJoints.w];
        modelMatrix = modelMatrix * skinMatrix;
    }

    WorldPos = vec3(modelMatrix * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}
