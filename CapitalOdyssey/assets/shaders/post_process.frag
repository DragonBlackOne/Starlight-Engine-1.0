#version 410 core
out vec4 FragColor;
  
in vec2 v_uv;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D depthTexture;
uniform float exposure;
uniform bool u_retro_mode;

uniform mat4 u_inv_view;
uniform mat4 u_inv_proj;
uniform mat4 u_view;
uniform mat4 u_proj;

// ACES Filmoc Tone Mapping
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

// --- RETRO GENESIS (Gen 3/4 Emulator Effects) ---
vec2 curveUV(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(4.0, 3.0);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

// SSR Helpers
vec3 getViewPos(vec2 uv, float z) {
    vec4 clip = vec4(uv * 2.0 - 1.0, z * 2.0 - 1.0, 1.0);
    vec4 view = u_inv_proj * clip;
    return view.xyz / view.w;
}

vec3 getNormal(vec3 viewPos) {
    return normalize(cross(dFdx(viewPos), dFdy(viewPos)));
}

vec3 calculateSSR(vec3 viewPos, vec3 viewNormal, vec2 uv, float rawDepth) {
    if (rawDepth >= 1.0) return vec3(0.0); // Skybox
    
    // Calcula Normal no Espaco Mundo pra ver se aponta pra cima (chao molhado)
    vec3 worldNormal = (u_inv_view * vec4(viewNormal, 0.0)).xyz;
    if (worldNormal.y < 0.8) return vec3(0.0); // Só reflete o chão

    vec3 viewDir = normalize(viewPos);
    vec3 reflected = normalize(reflect(viewDir, viewNormal));
    
    float maxDistance = 40.0;
    float resolution  = 0.3; // Passos do Raymarch
    int   steps       = 40;
    float thickness   = 0.5;

    vec3 rayPos = viewPos;
    vec2 hitUV = vec2(-1.0);
    
    for(int i = 0; i < steps; i++) {
        rayPos += reflected * resolution;
        
        vec4 projPos = u_proj * vec4(rayPos, 1.0);
        projPos.xyz /= projPos.w;
        vec2 sampleUV = projPos.xy * 0.5 + 0.5;
        
        if(sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            break;
            
        float sampleDepth = texture(depthTexture, sampleUV).r;
        vec3 sampleViewPos = getViewPos(sampleUV, sampleDepth);
        
        // Verifica se cruzou a geometria
        float depthDiff = rayPos.z - sampleViewPos.z;
        if(depthDiff > 0.0 && depthDiff < thickness) {
            hitUV = sampleUV;
            break;
        }
    }
    
    if (hitUV.x > 0.0) {
        // Atenuação das bordas da tela
        vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - hitUV.xy));
        float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
        return texture(screenTexture, hitUV).rgb * screenEdgefactor;
    }
    
    return vec3(0.0);
}

void main() {
    vec2 curved_uv = u_retro_mode ? curveUV(v_uv) : v_uv;
    vec3 hdrColor = vec3(0.0);
    vec3 bloomColor = vec3(0.0);
    float rawDepth = texture(depthTexture, curved_uv).r;
    
    // Linearize depth
    float zNear = 0.1;
    float zFar = 1000.0;
    float z = rawDepth * 2.0 - 1.0; 
    float linearDepth = (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
    float depth01 = clamp(linearDepth / zFar, 0.0, 1.0);
    
    // TV Bounds
    if (!u_retro_mode || (curved_uv.x >= 0.0 && curved_uv.x <= 1.0 && curved_uv.y >= 0.0 && curved_uv.y <= 1.0)) {
        if (u_retro_mode) {
            float bleed = 0.002;
            hdrColor.r = texture(screenTexture, curved_uv + vec2(bleed, 0.0)).r;
            hdrColor.g = texture(screenTexture, curved_uv).g;
            hdrColor.b = texture(screenTexture, curved_uv - vec2(bleed, 0.0)).b;
        } else {
            hdrColor = texture(screenTexture, curved_uv).rgb;
            
            // Feature 3: Screen Space Reflections (Chão Molhado/Metálico)
            vec3 viewPos = getViewPos(curved_uv, rawDepth);
            vec3 viewNormal = getNormal(viewPos);
            vec3 ssrColor = calculateSSR(viewPos, viewNormal, curved_uv, rawDepth);
            hdrColor += ssrColor * 0.45; // Mistura suave do reflexo
        }
        bloomColor = texture(bloomBlur, curved_uv).rgb;
    }
    
    // Feature 1: AAA Depth of Field (Using bloomBlur as out-of-focus background)
    // Se o depth is high (distant), we mix more of the blurred texture
    float dofFactor = smoothstep(0.05, 0.8, depth01); 
    hdrColor = mix(hdrColor, bloomColor, dofFactor * 0.85);

    // Feature 2: Volumetric Distance Fog
    vec3 fogColor = vec3(0.5, 0.6, 0.7); // Light scattering blueish tint
    float fogFactor = smoothstep(0.02, 1.0, depth01);
    
    // Combine HDR + Bloom (foreground bloom)
    hdrColor += bloomColor * 0.5; 
    
    // Apply Fog (Lerp towards fog color based on depth)
    hdrColor = mix(hdrColor, fogColor * 4.0, fogFactor); // Fog is very bright via HDR

    // Exposure tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    // ACES Filmic Tone Mapping
    result = ACESFilm(result);
    
    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));
    
    if (u_retro_mode && curved_uv.x >= 0.0 && curved_uv.x <= 1.0 && curved_uv.y >= 0.0 && curved_uv.y <= 1.0) {
        float colors = 16.0;
        result = floor(result * colors) / colors;
        float scanline = sin(curved_uv.y * 600.0) * 0.05;
        result -= scanline;
        result *= 1.1;
    }
    
    if (u_retro_mode) {
        vec2 uv = curved_uv * (1.0 - curved_uv.yx);
        float vig = uv.x * uv.y * 15.0;
        vig = pow(vig, 0.20);
        result *= vig;
    }

    FragColor = vec4(result, 1.0);
}
