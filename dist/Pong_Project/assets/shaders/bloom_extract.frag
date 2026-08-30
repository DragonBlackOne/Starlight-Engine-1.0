#version 330 core

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_texture;
uniform float u_threshold;

void main() {
    vec3 color = texture(u_texture, v_texcoord).rgb;
    
    // Calculate brightness (luminance)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // Extract only bright pixels
    if(brightness > u_threshold) {
        f_color = vec4(color, 1.0);
    } else {
        f_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
