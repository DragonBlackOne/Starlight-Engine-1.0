#version 410 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// Material Uniforms
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// Humanoid Skin / Subsurface Scattering Uniforms
uniform int isSkin;
uniform vec3 skinSubsurfaceColor;

// Texture Maps
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;
uniform int useAlbedoMap;
uniform int useNormalMap;
uniform int useMetallicRoughnessMap;

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
uniform int useIBL;

// Atmospheric distance fog
uniform vec3 fogColor;
uniform float fogDensity;
uniform int fogEnabled;

const float PI = 3.14159265359;

// Screen-Space Derivative Cotangent Frame for Normal Mapping (No mesh tangents needed)
mat3 CotangentFrame(vec3 N, vec3 p, vec2 uv) {
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);
    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, N);
}

float DistributionGGX(vec3 N, vec3 H, float r) {
    float a = r * r;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float r) {
    float k = ((r + 1.0) * (r + 1.0)) / 8.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 0.0000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float r) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, r);
    float ggx1 = GeometrySchlickGGX(NdotL, r);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float r) {
    return F0 + (max(vec3(1.0 - r), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0).xy);
    
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
    // 1. Resolve Base Material Attributes
    vec3 matAlbedo = (useAlbedoMap != 0) ? texture(albedoMap, TexCoords).rgb * albedo : albedo;
    float matMetallic = metallic;
    float matRoughness = roughness;
    float matAO = ao;

    if (useMetallicRoughnessMap != 0) {
        vec4 mr = texture(metallicRoughnessMap, TexCoords);
        matMetallic = mr.b * metallic;
        matRoughness = mr.g * roughness;
        matAO = mr.r * ao;
    }

    // 2. Normal Mapping (via screen-space derivative cotangent frame)
    vec3 N = normalize(Normal);
    if (useNormalMap != 0) {
        vec3 mapN = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
        mat3 TBN = CotangentFrame(N, WorldPos, TexCoords);
        N = normalize(TBN * mapN);
    }

    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, matAlbedo, matMetallic);

    // 3. Point Lights Contribution
    vec3 Lo = vec3(0.0);
    int activeLightCount = min(lightCount, 8);
    for(int i = 0; i < activeLightCount; ++i) {
        vec3 L = normalize(lights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - WorldPos);
        float attenuation = 1.0 / max(distance * distance, 0.01);
        vec3 radiance = lights[i].color * lights[i].intensity * attenuation;

        float NDF = DistributionGGX(N, H, matRoughness);   
        float G   = GeometrySmith(N, V, L, matRoughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - matMetallic;	  

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * matAlbedo / PI + specular) * radiance * NdotL;
    }   

    // 4. Main Directional Sun Light with CSM Soft Shadow
    vec3 LoDir = vec3(0.0);
    vec3 Ldir = normalize(-lightDir);
    vec3 Hdir = normalize(V + Ldir);
    vec3 dirRadiance = vec3(1.4, 1.35, 1.25) * 1.8; // Sunlight intensity

    float NDFdir = DistributionGGX(N, Hdir, matRoughness);   
    float Gdir   = GeometrySmith(N, V, Ldir, matRoughness);    
    vec3 Fdir    = fresnelSchlick(max(dot(Hdir, V), 0.0), F0);        
    
    vec3 numDir   = NDFdir * Gdir * Fdir;
    float denomDir = 4.0 * max(dot(N, V), 0.0) * max(dot(N, Ldir), 0.0) + 0.0001;
    vec3 specDir = numDir / denomDir;
    
    vec3 kSdir = Fdir;
    vec3 kDdir = (vec3(1.0) - kSdir) * (1.0 - matMetallic);

    float NdotLdir = max(dot(N, Ldir), 0.0);
    LoDir = (kDdir * matAlbedo / PI + specDir) * dirRadiance * NdotLdir;

    float shadow = CalculateShadow(WorldPos, N);

    // 5. Humanoid Subsurface Scattering (SSS) & Translucency
    vec3 SSS = vec3(0.0);
    if (isSkin != 0) {
        vec3 sssSubColor = (skinSubsurfaceColor == vec3(0.0)) ? vec3(0.92, 0.38, 0.25) : skinSubsurfaceColor;
        vec3 distortedLight = Ldir + N * 0.35;
        float sssDot = pow(clamp(dot(V, -distortedLight), 0.0, 1.0), 3.0);
        float wrapNdotL = max(dot(N, Ldir) * 0.5 + 0.5, 0.0); // Wrap lighting for soft skin
        SSS = (sssDot * 0.45 + wrapNdotL * 0.25) * sssSubColor * dirRadiance * (1.0 - matMetallic);
    }

    // 6. Ambient Lighting (IBL or Ground/Sky Gradient Fallback)
    vec3 ambient = vec3(0.0);
    if (useIBL != 0) {
        vec3 F_IBL = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, matRoughness);
        vec3 kS_IBL = F_IBL;
        vec3 kD_IBL = (vec3(1.0) - kS_IBL) * (1.0 - matMetallic);

        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuseIBL = irradiance * matAlbedo;

        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R, matRoughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), matRoughness)).rg;
        vec3 specularIBL = prefilteredColor * (F_IBL * envBRDF.x + envBRDF.y);

        ambient = (kD_IBL * diffuseIBL + specularIBL) * matAO;
    } else {
        vec3 skyColor = vec3(0.40, 0.48, 0.60);
        vec3 groundColor = vec3(0.24, 0.18, 0.14);
        float upFactor = N.y * 0.5 + 0.5;
        vec3 envAmbient = mix(groundColor, skyColor, upFactor) * 0.45;
        ambient = envAmbient * matAlbedo * matAO;
    }

    // 7. Final Combined Lighting with Deep Soft Shadows
    vec3 color = ambient + Lo + (1.0 - shadow * 0.78) * LoDir + SSS;

    // 8. Atmospheric Distance Fog (soft desert haze)
    if (fogEnabled != 0 && fogDensity > 0.0) {
        float dist = length(camPos - WorldPos);
        float fogFactor = 1.0 - exp(-fogDensity * dist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        color = mix(color, fogColor, fogFactor);
    }

    FragColor = vec4(color, 1.0);
}
