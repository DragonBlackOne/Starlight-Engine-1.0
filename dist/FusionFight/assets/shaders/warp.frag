
#version 330 core
out vec4 FragColor;
in vec2 v_uv;

uniform float u_time;

float hash(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }

void main() {
    vec2 uv = v_uv * 2.0 - 1.0;
    float r = length(uv);
    float angle = atan(uv.y, uv.x);
    
    // Streaks moving outwards
    float streaks = 0.0;
    float a = angle * 5.0 + u_time * 2.0;
    streaks = pow(sin(a + r * 10.0), 10.0) * (1.0 - r);
    
    // Core glow
    float core = exp(-r * 10.0);
    
    vec3 col = mix(vec3(0.2, 0.4, 1.0), vec3(0.8, 0.9, 1.0), streaks);
    col += core * vec3(1.0, 1.0, 0.8);
    
    FragColor = vec4(col, 1.0);
}
