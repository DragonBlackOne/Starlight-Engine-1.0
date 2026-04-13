#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D depthTexture;
uniform sampler2D ssaoTexture;

uniform float exposure = 1.0;
uniform bool u_retro_mode = false;

uniform mat4 u_inv_view;
uniform mat4 u_inv_proj;
uniform mat4 u_view;
uniform mat4 u_proj;

// ACES Filmic Tone Mapping
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec3 getViewPos(vec2 uv, float z) {
    vec4 clip = vec4(uv * 2.0 - 1.0, z * 2.0 - 1.0, 1.0);
    vec4 view = u_inv_proj * clip;
    return view.xyz / view.w;
}

vec3 getNormal(vec3 viewPos) {
    return normalize(cross(dFdx(viewPos), dFdy(viewPos)));
}

vec3 calculateSSR(vec3 viewPos, vec3 viewNormal, vec2 uv, float rawDepth) {
    if (rawDepth >= 1.0) return vec3(0.0);
    
    vec3 viewDir = normalize(viewPos);
    vec3 reflected = normalize(reflect(viewDir, viewNormal));
    
    float res = 0.4; // Step resolution
    int steps = 30;
    float thick = 0.4;
    vec3 rayPos = viewPos;
    
    for(int i = 0; i < steps; i++) {
        rayPos += reflected * res;
        vec4 proj = u_proj * vec4(rayPos, 1.0);
        proj.xyz /= proj.w;
        vec2 sampleUV = proj.xy * 0.5 + 0.5;
        
        if(sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0) break;
            
        float sampleDepth = texture(depthTexture, sampleUV).r;
        vec3 sampleViewPos = getViewPos(sampleUV, sampleDepth);
        
        float diff = rayPos.z - sampleViewPos.z;
        if(diff > 0.0 && diff < thick) {
            float fade = smoothstep(0.1, 0.5, 1.0 - abs(sampleUV.x - 0.5) * 2.0) * smoothstep(0.1, 0.5, 1.0 - abs(sampleUV.y - 0.5) * 2.0);
            return texture(screenTexture, sampleUV).rgb * fade;
        }
    }
    return vec3(0.0);
}

void main() {
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
    float rawDepth = texture(depthTexture, TexCoords).r;
    float ao = texture(ssaoTexture, TexCoords).r;
    
    // Apply AO early
    hdrColor *= ao;

    // 1. SSR
    vec3 viewPos = getViewPos(TexCoords, rawDepth);
    vec3 viewNormal = getNormal(viewPos);
    vec3 ssrColor = calculateSSR(viewPos, viewNormal, TexCoords, rawDepth);
    hdrColor += ssrColor * 0.3;

    // 2. Bloom
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor * 1.5;

    // 3. Volumetric Fog
    float zNear = 0.1;
    float zFar = 1000.0;
    float z = rawDepth * 2.0 - 1.0; 
    float linearDepth = (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
    float depth01 = clamp(linearDepth / 200.0, 0.0, 1.0); // Fog range 200 units
    vec3 fogColor = vec3(0.2, 0.3, 0.4);
    hdrColor = mix(hdrColor, fogColor, depth01 * 0.8);

    // 4. Tone Mapping & Gamma
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = ACESFilm(result);
    // result = pow(result, vec3(1.0 / 2.2)); // Handled by hardware sRGB if enabled, or here

    // 5. Retro Overlays (Toggleable)
    if (u_retro_mode) {
        float scanline = sin(TexCoords.y * 800.0) * 0.04;
        result -= scanline;
    }

    FragColor = vec4(result, 1.0);
}
