#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec2 gRoughnessAO;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 albedoColor;
uniform float metallic;
uniform float roughness;

void main()
{    
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = albedoColor;
    gAlbedoSpec.a = metallic;
    
    // Store roughness in R, AO in G (AO is 1.0 by default)
    gRoughnessAO.r = roughness;
    gRoughnessAO.g = 1.0; 
}
