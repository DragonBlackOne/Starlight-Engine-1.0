#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D shadowMap;

// Luz Direcional
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float lightIntensity;

// Camera
uniform vec3 viewPos;

// Material
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

// Point Lights (max 8)
#define MAX_POINT_LIGHTS 8
uniform int numPointLights;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];
uniform float pointLightRadius[MAX_POINT_LIGHTS];

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    
    // PCF 3x3
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

vec3 CalcPointLight(vec3 lightPos, vec3 lightCol, float radius, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightVec = lightPos - fragPos;
    float dist = length(lightVec);
    vec3 lightDirection = normalize(lightVec);
    
    // Atenuacao quadratica
    float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
    attenuation *= clamp(1.0 - dist / radius, 0.0, 1.0);
    
    // Diffuse
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightCol;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightCol;
    
    return (diffuse + specular) * attenuation;
}

void main()
{
    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Directional Light (Diffuse + Specular)
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * lightIntensity;
    
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor * lightIntensity;

    // Shadow
    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 directional = ambient + (1.0 - shadow) * (diffuse + specular);

    // Point Lights
    vec3 pointLighting = vec3(0.0);
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
        pointLighting += CalcPointLight(pointLightPos[i], pointLightColor[i], pointLightRadius[i], norm, FragPos, viewDir);
    }

    // Final
    vec3 texColor = texture(texture1, TexCoord).rgb;
    vec3 result = (directional + pointLighting) * texColor;
    
    // Atmospheric Fog (Ajustado para 0.002 para permitir visão de longa distância)
    float dist = length(viewPos - FragPos);
    float fogFactor = clamp(exp(-dist * 0.002), 0.0, 1.0); // Mais suave
    vec3 fogColor = vec3(0.05, 0.05, 0.08); // Mesmo que clear color
    result = mix(fogColor, result, fogFactor);

    FragColor = vec4(result, 1.0);
}
