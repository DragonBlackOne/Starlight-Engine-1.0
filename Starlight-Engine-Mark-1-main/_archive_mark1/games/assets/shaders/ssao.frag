#version 330 core

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_depth_texture;
uniform sampler2D u_noise_texture;

uniform mat4 u_projection;
uniform vec2 u_noise_scale;
uniform float u_radius;
uniform float u_bias;

// Generate sample kernel in shader (procedural)
vec3 getSample(int index) {
    // Simple procedural hemisphere samples
    float phi = float(index) * 2.399;  // Golden angle
    float cosTheta = 1.0 - (float(index) / 16.0);
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    
    float x = cos(phi) * sinTheta;
    float y = sin(phi) * sinTheta;
    float z = cosTheta;
    
    // Scale based on index
    float scale = float(index) / 16.0;
    scale = mix(0.1, 1.0, scale * scale);
    
    return vec3(x, y, z) * scale;
}

void main() {
    float depth = texture(u_depth_texture, v_texcoord).r;
    
    // Early exit if sky
    if (depth >= 0.9999) {
        f_color = vec4(1.0);
        return;
    }
    
    // Reconstruct view-space position
    vec4 clipSpacePos = vec4(v_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePos = inverse(u_projection) * clipSpacePos;
    vec3 fragPos = viewSpacePos.xyz / viewSpacePos.w;
    
    // Simplified normal (pointing towards camera)
    vec3 normal = normalize(-fragPos);
    
    // Random rotation from noise
    vec3 randomVec = texture(u_noise_texture, v_texcoord * u_noise_scale).xyz * 2.0 - 1.0;
    
    // TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // Sample and accumulate
    float occlusion = 0.0;
    int samples = 16;  // Reduced for performance
    
    for(int i = 0; i < samples; ++i) {
        vec3 sampleVec = TBN * getSample(i);
        vec3 samplePos = fragPos + sampleVec * u_radius;
        
        // Project to screen
        vec4 offset = u_projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Sample depth
        float sampleDepth = texture(u_depth_texture, offset.xy).r;
        
        // Reconstruct sample Z
        vec4 sampleClip = vec4(offset.xy * 2.0 - 1.0, sampleDepth * 2.0 - 1.0, 1.0);
        vec4 sampleView = inverse(u_projection) * sampleClip;
        float sampleZ = sampleView.z / sampleView.w;
        
        // Range check
        float rangeCheck = smoothstep(0.0, 1.0, u_radius / abs(fragPos.z - sampleZ));
        occlusion += (sampleZ >= samplePos.z + u_bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / float(samples));
    f_color = vec4(vec3(occlusion), 1.0);
}
