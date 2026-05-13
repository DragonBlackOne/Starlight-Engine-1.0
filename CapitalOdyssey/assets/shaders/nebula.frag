
#version 330 core

out vec4 FragColor;
in vec2 v_uv;

uniform vec3 u_view_pos;
uniform mat4 u_inv_proj;
uniform mat4 u_inv_view;
uniform float u_time;

// Noise function
float hash(float n) { return fract(sin(n) * 753.5453123); }
float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
                   mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
               mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
                   mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

float fbm(vec3 p) {
    float f = 0.0;
    f += 0.5000 * noise(p); p = p * 2.02;
    f += 0.2500 * noise(p); p = p * 2.03;
    f += 0.1250 * noise(p); p = p * 2.01;
    f += 0.0625 * noise(p);
    return f;
}

void main() {
    // 1. Convert UV to Ray
    vec4 clip_pos = vec4(v_uv * 2.0 - 1.0, -1.0, 1.0);
    vec4 view_pos = u_inv_proj * clip_pos;
    view_pos /= view_pos.w;
    vec4 world_pos = u_inv_view * view_pos;
    vec3 rd = normalize(world_pos.xyz - u_view_pos);
    
    // 2. Ray Marching
    vec3 ro = u_view_pos * 0.01; // Scaled for nebula
    float t = 0.0;
    vec3 res = vec3(0.0);
    
    for(int i=0; i<40; i++) {
        vec3 p = ro + rd * t;
        float d = fbm(p + u_time * 0.2);
        
        // Color mapping
        vec3 col = mix(vec3(0.05, 0.0, 0.1), vec3(0.3, 0.1, 0.4), d);
        col *= exp(-0.02 * t); // Fog
        
        float alpha = d * 0.02; // Reduced from 0.05
        res += col * alpha;
        
        t += 5.0; // Large steps for cosmic scale
    }
    
    FragColor = vec4(res, 1.0);
}
