#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gPosition;

uniform vec3 u_light_dir;
uniform vec3 u_view_pos;
uniform mat4 m_proj;
uniform mat4 m_view;

// SSGI Settings
const int SAMPLES = 16;
const float RADIUS = 0.5;
const float BIAS = 0.05;
const float INTENSITY = 3.0; // Increased intensity

// Random noise
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    // 1. Retrieve data from G-Buffer
    vec4 albedoSample = texture(gAlbedo, TexCoords);
    vec3 albedo = albedoSample.rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    
    // Check if we hit the sky (using alpha or position)
    // If normal is 0, it's likely background
    if (length(normal) < 0.1) {
        // It's the sky/background. Just output albedo (which should be clear color if we cleared gAlbedo properly? 
        // Actually gAlbedo was cleared to black. 
        // But we cleared the SCREEN with sky color in main.py.
        // However, we are drawing a full screen quad OVER the screen.
        // We need to discard or output a sky color here if we want to see the background.
        // OR, we can rely on depth test? No, we disabled depth test for the quad.
        // We should probably discard if it's background, letting the clear color show through?
        // But we are writing to a texture (light_buffer), not the screen directly yet.
        // Let's assume position (0,0,0) is invalid/sky.
        if (length(fragPos) == 0.0) {
            discard; // Let the clear color (sky) show through
        }
    }
    
    // 2. Direct Lighting (Blinn-Phong)
    vec3 lightDir = normalize(u_light_dir);
    vec3 viewDir = normalize(u_view_pos - fragPos);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 sunColor = vec3(1.0, 0.95, 0.8); // Warm Sun
    vec3 diffuse = diff * sunColor;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.1) * spec;
    
    // Ambient (Sky Light)
    vec3 ambientColor = vec3(0.6, 0.7, 0.9); // Cool Sky
    vec3 ambient = ambientColor * 0.4;
    
    // 3. SSGI (Indirect Lighting)
    vec3 indirect = vec3(0.0);
    
    // TBN
    vec3 randomVec = normalize(vec3(rand(TexCoords), rand(TexCoords + 0.1), 0.0));
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    for(int i = 0; i < SAMPLES; ++i) {
        float r1 = rand(TexCoords + float(i) * 0.1);
        float r2 = rand(TexCoords + float(i) * 0.2);
        
        // Hemisphere sample
        float theta = 2.0 * 3.14159 * r1;
        float phi = acos(sqrt(1.0 - r2));
        
        vec3 samplePos = vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
        samplePos = TBN * samplePos;
        samplePos = fragPos + samplePos * RADIUS;
        
        // Project
        vec4 offset = vec4(samplePos, 1.0);
        offset = m_proj * m_view * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Range check
        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0) continue;
        
        // Sample Occluder
        vec3 occluderPos = texture(gPosition, offset.xy).rgb;
        
        // Check if valid occluder
        if (length(occluderPos) == 0.0) continue; // Sky doesn't occlude/bounce light in this simple model
        
        // Distance Check (World Space)
        float distSample = distance(u_view_pos, samplePos);
        float distOccluder = distance(u_view_pos, occluderPos);
        
        // Range check to avoid self-shadowing artifacts or distant objects
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(distSample - distOccluder));
        
        // If occluder is closer to camera than sample, it's an occlusion/bounce surface
        if (distOccluder < distSample - BIAS) {
            // Get color from occluder (Color Bleeding)
            vec3 occluderAlbedo = texture(gAlbedo, offset.xy).rgb;
            vec3 occluderNormal = texture(gNormal, offset.xy).rgb;
            
            // Is occluder lit?
            float occluderLit = max(dot(occluderNormal, lightDir), 0.0);
            
            indirect += occluderAlbedo * occluderLit * rangeCheck;
        }
    }
    
    indirect = (indirect / float(SAMPLES)) * INTENSITY;
    
    // Combine
    vec3 lighting = (ambient + diffuse + indirect) * albedo + specular;
    
    // --- POST PROCESSING (Vibrance & Tone Mapping) ---
    
    // 1. Vibrance
    float luminance = dot(lighting, vec3(0.299, 0.587, 0.114));
    vec3 gray = vec3(luminance);
    vec3 saturated = mix(gray, lighting, 1.5); // 50% saturation boost
    lighting = mix(gray, saturated, 1.0);
    
    // 2. Tone Mapping (ACES approximation for nice colors)
    lighting = lighting * 0.6; // Exposure
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    lighting = clamp((lighting*(a*lighting+b))/(lighting*(c*lighting+d)+e), 0.0, 1.0);
    
    // 3. Gamma
    lighting = pow(lighting, vec3(1.0 / 2.2));
    
    FragColor = vec4(lighting, 1.0);
}
