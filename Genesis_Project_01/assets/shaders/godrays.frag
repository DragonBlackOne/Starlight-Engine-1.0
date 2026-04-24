#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gDepth;
uniform vec2 u_sun_pos; // Screen space (0..1)
uniform float u_exposure;
uniform float u_decay;
uniform float u_density;
uniform float u_weight;

const int SAMPLES = 64;

void main() {
    vec2 texCoord = TexCoords;
    vec2 deltaTextCoord = vec2(texCoord - u_sun_pos);
    
    deltaTextCoord *= 1.0 / float(SAMPLES) * u_density;
    
    float illuminationDecay = 1.0;
    vec3 color = vec3(0.0);
    
    for(int i = 0; i < SAMPLES; i++) {
        texCoord -= deltaTextCoord;
        
        // Sample Depth
        float depth = texture(gDepth, texCoord).r;
        
        // Identify Light Source (Sky)
        // In OpenGL default depth is 0..1 (1 is far).
        // If depth is 1.0, it's sky.
        if (depth >= 0.9999) {
             color += vec3(1.0, 0.9, 0.7) * u_weight * illuminationDecay;
        }
        
        illuminationDecay *= u_decay;
    }
    
    FragColor = vec4(color * u_exposure, 1.0);
}
