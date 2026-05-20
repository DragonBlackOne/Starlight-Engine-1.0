#version 410 core
out vec4 outColor;
in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform vec2 lightScreenPos;
uniform float density = 0.96;
uniform float weight = 0.587;
uniform float decay = 0.9;
uniform float exposure = 0.2;

const int SAMPLES = 64;

void main() {
    vec2 deltaTexCoord = (TexCoords - lightScreenPos);
    deltaTexCoord *= 1.0 / float(SAMPLES) * density;
    
    vec3 color = texture(sceneTexture, TexCoords).rgb;
    float illuminationDecay = 1.0;
    
    vec2 texCoord = TexCoords;
    for (int i = 0; i < SAMPLES; i++) {
        texCoord -= deltaTexCoord;
        vec3 sampleColor = texture(sceneTexture, texCoord).rgb;
        sampleColor *= illuminationDecay * weight;
        color += sampleColor;
        illuminationDecay *= decay;
    }
    
    outColor = vec4(color * exposure, 1.0);
}
