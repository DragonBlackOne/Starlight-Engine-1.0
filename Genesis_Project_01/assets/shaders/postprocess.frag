#version 330 core

in vec2 v_uv;
out vec4 f_color;

uniform sampler2D scene;
uniform sampler2D bloom;
uniform float u_bloom_strength; 
uniform float screen_shake; 
uniform bool vignette_enabled;
uniform float u_hyper_mode; 
uniform int quality; 
uniform float time; 

vec2 curve(vec2 uv) {
    uv = (uv - 0.5) * 2.0;
    uv *= 1.1;	
    uv.x *= 1.0 + pow((abs(uv.y) / 5.0), 2.0);
    uv.y *= 1.0 + pow((abs(uv.x) / 4.0), 2.0);
    uv  = (uv / 2.0) + 0.5;
    uv =  uv *0.92 + 0.04;
    return uv;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec2 q = v_uv;
    vec2 uv = curve(q);
    
    vec3 color = vec3(0.0);
    
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        color = vec3(0.0);
    } else {
        // Chromatic Aberration + Glitch
        float spread = (screen_shake * 0.05) + 0.002;
        float r = texture(scene, uv + vec2(spread, 0.0)).r;
        float g = texture(scene, uv).g;
        float b = texture(scene, uv - vec2(spread, 0.0)).b;
        
        vec3 hdrColor = vec3(r, g, b);

        // Advanced Bloom Mix
        vec3 bloomColor = texture(bloom, uv).rgb;
        float b_strength = mix(1.0, u_bloom_strength, 0.5); // Fallback if 0
        hdrColor += bloomColor * b_strength;
        
        // ACES Filmic Tone Mapping
        float A = 2.51;
        float B = 0.03;
        float C = 2.43;
        float D = 0.59;
        float E = 0.14;
        color = clamp((hdrColor * (A * hdrColor + B)) / (hdrColor * (C * hdrColor + D) + E), 0.0, 1.0);
        
        // Gamma Correct
        color = pow(color, vec3(1.0 / 2.2));
        
        // Hyper Mode
        if (u_hyper_mode > 0.0) {
            vec3 inverted = vec3(1.0) - color;
            float pulse = 0.5 + 0.5 * sin(time * 20.0);
            color = mix(color, inverted, u_hyper_mode * pulse);
            color += vec3(0.2, 0.0, 0.0) * u_hyper_mode;
        }
        
        // Cinematic Film Grain
        float grain = (rand(uv + time) - 0.5) * 0.08;
        color += grain;
        
        // Retro Scanlines
        float scans = clamp(0.35 + 0.35 * sin(3.5 * time + uv.y * 720.0 * 1.5), 0.0, 1.0);
        float s = pow(scans, 1.7);
        color = color * vec3(0.85 + 0.15 * s); 
        
        // Vignette
        if (vignette_enabled) {
            float vig = (16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y));
            vig = pow(vig, 0.15); 
            color *= vig;
        }
    }

    f_color = vec4(color, 1.0);
}
