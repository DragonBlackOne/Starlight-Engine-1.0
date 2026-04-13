#version 450 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform vec3 uColor;
uniform sampler2D uTexture;
uniform bool uUseTexture;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(norm, lightDir), 0.2);
    
    vec3 baseColor = uColor;
    if (uUseTexture) {
        baseColor *= texture(uTexture, TexCoords).rgb;
    }
    
    vec3 finalColor = baseColor * diff;
    FragColor = vec4(finalColor, 1.0);
}
