#version 410 core

in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;

uniform vec4 color;
uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    // Ambient
    vec3 ambient = 0.25 * color.rgb;
    
    // Diffuse (Lambert)
    vec3 norm = normalize(vNormal);
    vec3 ld = normalize(-lightDir);
    float diff = max(dot(norm, ld), 0.0);
    vec3 diffuse = diff * lightColor * color.rgb;
    
    // Rim light (fake fresnel for neon look)
    vec3 viewDir = normalize(-vFragPos);
    float rim = 1.0 - max(dot(norm, viewDir), 0.0);
    rim = pow(rim, 3.0) * 0.3;
    vec3 rimColor = color.rgb * rim;
    
    vec3 result = ambient + diffuse * 0.7 + rimColor;
    // Transmite cor de forma pura e linar para o PostProcess
    FragColor = vec4(result, color.a);
}
