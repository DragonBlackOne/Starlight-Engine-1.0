#version 330 core

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_texture;
uniform sampler2D u_depth_texture;

void main() {
    vec2 texelSize = 1.0 / textureSize(u_texture, 0);
    float result = 0.0;
    float centerDepth = texture(u_depth_texture, v_texcoord).r;
    float totalWeight = 0.0;
    
    // 4x4 bilateral blur
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sampleDepth = texture(u_depth_texture, v_texcoord + offset).r;
            
            // Weight based on depth similarity (edge preservation)
            float depthDiff = abs(centerDepth - sampleDepth);
            float weight = exp(-depthDiff * 100.0);  // Sharp falloff at edges
            
            result += texture(u_texture, v_texcoord + offset).r * weight;
            totalWeight += weight;
        }
    }
    
    result /= totalWeight;
    f_color = vec4(vec3(result), 1.0);
}
