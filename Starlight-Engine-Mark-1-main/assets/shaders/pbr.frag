
#version 410 core

layout (location = 0) out vec4 outColor;

in vec3 v_normal;
in vec3 v_frag_pos;
in vec2 v_uv;
in mat3 v_tbn;

// Material factors
uniform vec4  u_base_color;
uniform float u_metallic;
uniform float u_roughness;
uniform vec3  u_emissive_factor;

// Texture mapping
uniform bool      u_is_textured;
uniform sampler2D u_albedo_map;
uniform sampler2D u_normal_map;
uniform sampler2D u_metallic_roughness_map;
uniform sampler2D u_emissive_map;
uniform sampler2D u_ao_map;
uniform bool      u_use_ibl;
uniform samplerCube u_irradiance_map;
uniform samplerCube u_prefilter_map;
uniform sampler2D   u_brdf_lut;
uniform sampler2D   u_ssao_map;
uniform bool        u_use_ssao;
uniform sampler2DArrayShadow u_shadow_map;
uniform mat4             u_light_matrices[4];
uniform float            u_cascade_splits[4];

// Singularity: Ray-Traced Shadows
uniform vec3  u_planet_pos[10];
uniform float u_planet_radius[10];
uniform int   u_planet_count;

// Lights
uniform vec3 u_light_dir;
uniform vec3 u_light_color;
uniform vec3 u_view_pos;

const float PI = 3.14159265359;

// Ray-Sphere intersection for shadows
float raySphereIntersect(vec3 ro, vec3 rd, vec3 pos, float radius) {
    vec3 oc = ro - pos;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;
    float h = b * b - c;
    if(h < 0.0) return -1.0;
    return -b - sqrt(h);
}

float calculateShadow(vec3 p, vec3 N) {
    if (length(u_light_dir) < 0.001) return 1.0;
    vec3 rd = normalize(u_light_dir); 
    for(int i = 0; i < u_planet_count; i++) {
        float d = raySphereIntersect(p + N * 0.01, rd, u_planet_pos[i], u_planet_radius[i]);
        if(d > 0.0) return 0.0;
    }
    return 1.0;
}

// PBR Functions
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

// ----------------------------------------------------------------------------
// PCSS (Percentage-Closer Soft Shadows) Implementation
// Provides realistic variable-width shadow penumbrae
// ----------------------------------------------------------------------------

// Poisson disk samples for shadow filtering
const vec2 POISSON_DISK[16] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790)
);

// Light size for PCSS (tune based on scene scale)
const float LIGHT_SIZE = 0.02;
const float MIN_PENUMBRA = 0.5;  // Minimum penumbra width
const float MAX_PENUMBRA = 15.0; // Maximum penumbra width

float random(vec3 seed, int i) {
    vec4 seed4 = vec4(seed, float(i));
    float dot_prod = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_prod) * 43758.5453);
}

// Blocker search: Find average depth of occluders
float findBlockerDepth(vec3 projCoords, int layer, float searchRadius) {
    float blockerSum = 0.0;
    int blockerCount = 0;
    
    for (int i = 0; i < 16; i++) {
        vec2 offset = POISSON_DISK[i] * searchRadius;
        float sampleDepth = texture(u_shadow_map, vec4(projCoords.xy + offset, layer, 1.0)).r;
        
        if (sampleDepth < projCoords.z - 0.002) {
            blockerSum += sampleDepth;
            blockerCount++;
        }
    }
    
    return (blockerCount > 0) ? blockerSum / float(blockerCount) : -1.0;
}

// PCSS shadow calculation
float calculate_shadow_pcss(vec3 fragPosWorld, vec3 normal) {
    float dist = length(u_view_pos - fragPosWorld);
    
    // Select cascade
    int layer = -1;
    for (int i = 0; i < 4; i++) {
        if (dist < u_cascade_splits[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) layer = 3;

    // Transform to light space
    vec4 fragPosLightSpace = u_light_matrices[layer] * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    vec2 texelSize = 1.0 / vec2(2048.0);
    float currentDepth = projCoords.z;
    
    // Step 1: Blocker search
    float searchRadius = LIGHT_SIZE * (currentDepth - 0.01) / currentDepth;
    float avgBlockerDepth = findBlockerDepth(projCoords, layer, searchRadius * 10.0);
    
    // No blockers = fully lit
    if (avgBlockerDepth < 0.0) return 0.0;
    
    // Step 2: Penumbra estimation
    float penumbraWidth = (currentDepth - avgBlockerDepth) * LIGHT_SIZE / avgBlockerDepth;
    penumbraWidth = clamp(penumbraWidth * 100.0, MIN_PENUMBRA, MAX_PENUMBRA);
    
    // Step 3: PCF with variable kernel size
    float shadow = 0.0;
    float filterRadius = penumbraWidth * texelSize.x;
    
    for (int i = 0; i < 16; i++) {
        vec2 offset = POISSON_DISK[i] * filterRadius;
        // Add rotation for temporal stability
        float angle = random(fragPosWorld, i) * 6.283185;
        mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
        offset = rotation * offset;
        
        shadow += texture(u_shadow_map, vec4(projCoords.xy + offset, layer, projCoords.z));
    }
    
    return shadow / 16.0;
}

// Standard PCF shadow (faster fallback)
float calculate_shadow(vec3 fragPosWorld, vec3 normal) {
    float dist = length(u_view_pos - fragPosWorld);
    
    int layer = -1;
    for (int i = 0; i < 4; i++) {
        if (dist < u_cascade_splits[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) layer = 3;

    vec4 fragPosLightSpace = u_light_matrices[layer] * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    // 3x3 PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(2048.0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            shadow += texture(u_shadow_map, vec4(projCoords.xy + vec2(x, y) * texelSize, layer, projCoords.z));
        }    
    }
    return shadow / 9.0;
}

void main() {
    // 1. Normal Mapping
    vec3 N;
    if (u_is_textured) {
        vec3 normal_map = texture(u_normal_map, v_uv).rgb;
        if (length(normal_map) > 0.01) {
            N = normalize(v_tbn * (normal_map * 2.0 - 1.0));
        } else {
            N = normalize(v_normal);
        }
    } else {
        N = normalize(v_normal);
    }

    vec3 V = normalize(u_view_pos - v_frag_pos);
    float shadow = calculateShadow(v_frag_pos, N);

    // 2. Material properties
    vec3 albedo = u_base_color.rgb;
    float metallic = u_metallic;
    float roughness = u_roughness;
    float ao = 1.0;

    if (u_is_textured) {
        albedo *= pow(texture(u_albedo_map, v_uv).rgb, vec3(2.2));
        vec3 mr = texture(u_metallic_roughness_map, v_uv).rgb;
        metallic *= mr.b;
        roughness *= mr.g;
        ao = texture(u_ao_map, v_uv).r;
    }

    // 3. PBR Lighting (Cook-Torrance BRDF)
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    vec3 L = normalize(u_light_dir);
    vec3 H = normalize(V + L);
    
    // Feature: Soft Shadows/PCF (Percentage-Closer Soft Shadows)
    float shadow_val = calculate_shadow_pcss(v_frag_pos, N);
    vec3 radiance = u_light_color * 10.0 * (1.0 - shadow_val); 

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

    // 4. Global Illumination & Emissive
    vec3 ambient;
    if (u_use_ibl) {
        vec3 F_ibl = fresnelSchlick(max(dot(N, V), 0.0), F0);
        vec3 kS_ibl = F_ibl;
        vec3 kD_ibl = 1.0 - kS_ibl;
        kD_ibl *= 1.0 - metallic;

        vec3 irradiance = texture(u_irradiance_map, N).rgb;
        vec3 diffuse_ibl = irradiance * albedo;

        // Sample both prefilter map and BRDF lut for specular IBL
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 R = reflect(-V, N);
        vec3 prefilteredColor = textureLod(u_prefilter_map, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf = texture(u_brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular_ibl = prefilteredColor * (F_ibl * brdf.x + brdf.y);

        ambient = (kD_ibl * diffuse_ibl + specular_ibl) * ao;
        if (u_use_ssao) {
            float ssao = texture(u_ssao_map, gl_FragCoord.xy / textureSize(u_ssao_map, 0)).r;
            ambient *= ssao;
        }
    } else {
        ambient = vec3(0.03) * albedo * ao;
        if (u_use_ssao) {
            float ssao = texture(u_ssao_map, gl_FragCoord.xy / textureSize(u_ssao_map, 0)).r;
            ambient *= ssao;
        }
    }
    
    vec3 emissive = u_emissive_factor;
    if (u_is_textured) {
        emissive *= texture(u_emissive_map, v_uv).rgb;
    }
    
    vec3 color = ambient + Lo + emissive;
    outColor = vec4(color, 1.0);
}
