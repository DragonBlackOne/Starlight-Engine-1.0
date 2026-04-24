#version 330 core

in vec3 v_color;
in vec3 v_normal;
in float v_wind_factor;

out vec4 f_color;

uniform vec3 u_light_dir;

void main() {
    // Simple diffuse lighting
    vec3 light_dir = normalize(u_light_dir);
    float diff = max(dot(v_normal, light_dir), 0.2); // Ambient min
    
    vec3 color = v_color * diff;
    
    // Slight brightness variation based on wind
    color += vec3(0.05) * v_wind_factor * 0.5;
    
    f_color = vec4(color, 1.0);
}
