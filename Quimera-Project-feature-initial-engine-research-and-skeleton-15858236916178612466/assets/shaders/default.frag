#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec4 u_BaseColor;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

uniform sampler2D u_ShadowMap;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 fragPosLightSpace) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    // Bias to prevent shadow acne
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // PCF (Percentage Closer Filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
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
void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(u_ViewPos - FragPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, u_BaseColor.rgb, u_Metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    
    // Light calculation
    vec3 L = normalize(u_LightPos - FragPos);
    vec3 H = normalize(V + L);
    float distance = length(u_LightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = vec3(10.0) * attenuation; // Base light color

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, u_Roughness);   
    float G   = GeometrySmith(N, V, L, u_Roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_Metallic;	  

    float NdotL = max(dot(N, L), 0.0);        
    float shadow = ShadowCalculation(FragPosLightSpace);
    Lo += ((kD * u_BaseColor.rgb / PI + specular) * radiance * NdotL) * (1.0 - shadow);

    // Ambient lighting (Faux-IBL)
    vec3 ambient = vec3(0.03) * u_BaseColor.rgb * u_AO;
    vec3 color = ambient + Lo;

    // Tone mapping (ACES Approximation)
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
