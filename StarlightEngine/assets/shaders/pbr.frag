#version 430 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 camPos;

// Lights
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};
uniform Light lights[8];
uniform int lightCount;

// CSM Soft Shadows
uniform sampler2DArray shadowMap;
uniform mat4 lightSpaceMatrices[4];
uniform float cascadePlaneDistances[4];
uniform vec3 lightDir;
uniform mat4 view;

// IBL Maps
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float CalculateShadow(vec3 fragPosWorld, vec3 normal) {
    vec4 fragPosView = view * vec4(fragPosWorld, 1.0);
    float depthValue = abs(fragPosView.z);

    int layer = -1;
    for (int i = 0; i < 4; ++i) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = 3;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) {
        return 0.0;
    }

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    
    // Sloped depth bias based on light direction
    float bias = max(0.05 * (1.0 - dot(normal, -lightDir)), 0.005);
    if (layer == 3) {
        bias *= 0.1;
    }

    int halfRange = 1;
    for(int x = -halfRange; x <= halfRange; ++x) {
        for(int y = -halfRange; y <= halfRange; ++y) {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r; 
            shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // 1. Point lights contribution
    vec3 Lo = vec3(0.0);
    int activeLightCount = min(lightCount, 8);
    for(int i = 0; i < activeLightCount; ++i) {
        vec3 L = normalize(lights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lights[i].color * lights[i].intensity * attenuation;

        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    
    // 2. Main Directional Light with CSM soft shadow
    vec3 LoDir = vec3(0.0);
    vec3 Ldir = normalize(-lightDir);
    vec3 Hdir = normalize(V + Ldir);
    vec3 dirRadiance = vec3(1.5, 1.45, 1.35) * 1.5; // Sunlight intensity

    float NDFdir = DistributionGGX(N, Hdir, roughness);   
    float Gdir   = GeometrySmith(N, V, Ldir, roughness);    
    vec3 Fdir    = fresnelSchlick(max(dot(Hdir, V), 0.0), F0);        
    
    vec3 numDir   = NDFdir * Gdir * Fdir;
    float denomDir = 4.0 * max(dot(N, V), 0.0) * max(dot(N, Ldir), 0.0) + 0.0001;
    vec3 specDir = numDir / denomDir;
    
    vec3 kSdir = Fdir;
    vec3 kDdir = vec3(1.0) - kSdir;
    kDdir *= 1.0 - metallic;	  

    float NdotLdir = max(dot(N, Ldir), 0.0);
    LoDir = (kDdir * albedo / PI + specDir) * dirRadiance * NdotLdir;

    float shadow = CalculateShadow(WorldPos, N);

    // 3. IBL Ambient Lighting
    vec3 F_IBL = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS_IBL = F_IBL;
    vec3 kD_IBL = vec3(1.0) - kS_IBL;
    kD_IBL *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * albedo;

    vec3 R = reflect(-V, N);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F_IBL * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD_IBL * diffuseIBL + specularIBL) * ao;

    // Combine all lighting sources
    vec3 color = ambient + Lo + (1.0 - shadow) * LoDir;

    FragColor = vec4(color, 1.0);
}
