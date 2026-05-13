
#version 330 core
out vec4 FragColor;

in vec3 v_normal;
in vec3 v_frag_pos;
in vec2 v_uv;
in float v_height;

uniform vec3 u_view_pos;
uniform vec3 u_light_dir;
uniform sampler2D u_texture;
uniform bool u_is_textured;

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_light_dir);
    float diff = max(dot(N, L), 0.1);
    
    // Color based on height
    vec3 color;
    if (v_height < -0.05) color = vec3(0.0, 0.1, 0.4); // Deep Sea
    else if (v_height < 0.0) color = vec3(0.0, 0.4, 0.8); // Sea
    else if (v_height < 0.1) color = vec3(0.1, 0.6, 0.1); // Grass
    else if (v_height < 0.25) color = vec3(0.4, 0.3, 0.2); // Dirt
    else color = vec3(0.9, 0.9, 1.0); // Snow
    
    if(u_is_textured) {
        color *= texture(u_texture, v_uv).rgb;
    }

    FragColor = vec4(color * diff, 1.0);
}
