#version 330 core

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_texture;
uniform vec2 u_direction; // (1, 0) for horizontal, (0, 1) for vertical

void main() {
    vec2 tex_offset = 1.0 / textureSize(u_texture, 0); // gets size of single texel
    vec3 result = texture(u_texture, v_texcoord).rgb * 0.227027; // center weight
    
    // 5-tap Gaussian blur
    vec3 weights[2] = vec3[](vec3(0.1945946), vec3(0.1216216));
    
    for(int i = 1; i < 3; ++i) {
        result += texture(u_texture, v_texcoord + u_direction * tex_offset * float(i)).rgb * weights[i-1];
        result += texture(u_texture, v_texcoord - u_direction * tex_offset * float(i)).rgb * weights[i-1];
    }
    
    f_color = vec4(result, 1.0);
}
