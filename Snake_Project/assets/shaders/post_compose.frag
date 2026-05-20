#version 330 core
out vec4 outColor;
in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float exposure;
uniform float gamma;

// ACES Filmic Tone Mapping Curve
vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

uniform sampler2D ssaoTexture;
uniform bool useSSAO;

void main() {
    vec3 hdrColor = texture(sceneTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;
    
    if (useSSAO) {
        float ao = texture(ssaoTexture, TexCoords).r;
        hdrColor *= ao;
    }
    
    // Additive Bloom
    hdrColor += bloomColor;
    
    // ACES Tonemapping
    vec3 result = ACESFilm(hdrColor * exposure);
    
    // Gamma correction
    result = pow(result, vec3(1.0 / gamma));
    
    outColor = vec4(result, 1.0);
}
