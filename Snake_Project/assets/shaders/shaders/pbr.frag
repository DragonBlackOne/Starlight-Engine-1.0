#version 410 core
layout (location = 0) out vec4 outColor;

in vec3 v_normal;
in vec3 v_frag_pos;
in vec2 v_uv;
in float v_clip_depth;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform sampler2D albedoMap;
uniform bool useAlbedoMap;

uniform vec3 camPos;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[8];
uniform int lightCount;

// --- CSM Shadow Uniforms ---
uniform sampler2DArray shadowMap;
uniform mat4 lightSpaceMatrices[4];
uniform float cascadePlaneDistances[4];
uniform vec3 lightDir;

const float PI = 3.14159265359;

// --- Shadow Calculation with PCF ---
float ShadowCalculation(vec3 fragPosWorld) {
    // Determine which cascade to use based on view-space depth
    int cascade = 3;
    for (int i = 0; i < 4; ++i) {
        if (v_clip_depth < cascadePlaneDistances[i]) {
            cascade = i;
            break;
        }
    }

    // Transform fragment to light space for the chosen cascade
    vec4 fragPosLight = lightSpaceMatrices[cascade] * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5; // [-1,1] -> [0,1]

    // If outside shadow map, no shadow
    if (projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;

    // Bias to reduce shadow acne (larger for distant cascades)
    float bias = max(0.005 * (1.0 - dot(normalize(v_normal), normalize(-lightDir))), 0.0005);
    bias *= 1.0 / (cascadePlaneDistances[cascade] * 0.5);
    bias = clamp(bias, 0.0, 0.01);

    // PCF 3x3 for soft shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, float(cascade))).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

// --- PBR Functions ---
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
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

void main() {
    vec3 texAlbedo = albedo;
    if(useAlbedoMap) {
        texAlbedo *= texture(albedoMap, v_uv).rgb;
    }

    vec3 N = normalize(v_normal);
    vec3 V = normalize(camPos - v_frag_pos);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, texAlbedo, metallic);

    // Calculate shadow factor
    float shadow = ShadowCalculation(v_frag_pos);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightCount; ++i) {
        vec3 L = normalize(lights[i].position - v_frag_pos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - v_frag_pos);
        float attenuation = lights[i].intensity / (distance * distance);
        vec3 radiance = lights[i].color * attenuation;

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
        Lo += (kD * texAlbedo / PI + specular) * radiance * NdotL;
    }   
    
    // Apply shadow to direct lighting (shadow darkens Lo, not ambient)
    Lo *= (1.0 - shadow * 0.7); // Keep 30% light even in full shadow for visual quality

    vec3 ambient = vec3(0.03) * texAlbedo * ao;
    vec3 color = ambient + Lo;

    outColor = vec4(color, 1.0);
}
