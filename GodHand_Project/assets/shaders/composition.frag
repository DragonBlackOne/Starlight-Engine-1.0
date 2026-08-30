#version 330 core
out vec4 outColor;
in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float exposure;
uniform float gamma;
uniform float chromaticStrength;
uniform float vignetteStrength;

// CRT Uniforms
uniform int useCRT;
uniform float uTime;

// Distorção de barril / curvatura do monitor de tubo Timothy Lottes
vec2 Warp(vec2 pos) {
    pos = pos * 2.0 - 1.0;    
    pos *= vec2(1.0 + (pos.y * pos.y) * 0.03, 1.0 + (pos.x * pos.x) * 0.04);
    return pos * 0.5 + 0.5;
}

// ACES Filmic Tone Mapping
vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main()
{
    vec2 uv = TexCoords;
    if (useCRT != 0) {
        uv = Warp(TexCoords);
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
    }

    // Radial Chromatic Aberration
    vec2 toCenter = uv - vec2(0.5);
    vec2 redOffset = toCenter * chromaticStrength;
    vec2 blueOffset = -toCenter * chromaticStrength;

    vec3 sceneSample = vec3(
        texture(sceneTexture, uv + redOffset).r,
        texture(sceneTexture, uv).g,
        texture(sceneTexture, uv + blueOffset).b
    );
    vec3 bloomSample = vec3(
        texture(bloomTexture, uv + redOffset).r,
        texture(bloomTexture, uv).g,
        texture(bloomTexture, uv + blueOffset).b
    );

    vec3 hdrColor = sceneSample + bloomSample * 0.35;

    // ACES Filmic Tone Mapping directly on HDR linear color
    vec3 mapped = ACESFilm(hdrColor * exposure);
    
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    // Retro CRT Scanlines and Phosphor Mask (applied to LDR to look organic)
    if (useCRT != 0) {
        float scanline = sin(uv.y * 900.0 + uTime * 2.0) * 0.08;
        mapped -= scanline * mapped; // Escurece nas scanlines proporcionalmente ao brilho
        
        float phosphor = sin(uv.x * 1200.0) * 0.02;
        mapped += phosphor * mapped;
    }

    // Vignette
    float distFromCenter = length(toCenter);
    float vignette = 1.0 - (distFromCenter * vignetteStrength);
    mapped *= clamp(vignette, 0.0, 1.0);

    outColor = vec4(mapped, 1.0);
}
