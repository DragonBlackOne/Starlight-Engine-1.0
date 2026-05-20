#version 330 core
out vec4 outColor;
in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float exposure;
uniform float gamma;

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
    vec3 hdrColor = texture(sceneTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;
    
    // Additive blending
    hdrColor += bloomColor;

    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    
    // ACES Filmic Tone Mapping
    mapped = ACESFilm(mapped);
    
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    outColor = vec4(mapped, 1.0);
}
