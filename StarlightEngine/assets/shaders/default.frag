#version 330 core

in vec2 v_texcoord;
in vec3 v_normal;
in vec4 v_shadow_coord;

out vec4 f_color;

uniform sampler2D u_texture;
uniform sampler2DShadow u_shadow_map;
uniform vec3 u_light_pos;

void main() {
    // 1. Textura Base
    vec4 color = texture(u_texture, v_texcoord);
    
    // 2. Iluminação Simples (Lambert)
    vec3 normal = normalize(v_normal);
    vec3 light_dir = normalize(u_light_pos); // Luz direcional
    float diff = max(dot(normal, light_dir), 0.0);
    
    // 3. Sombra (PCF - Percentage Closer Filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);
    
    // Amostragem 3x3 para suavizar bordas
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            shadow += textureProj(u_shadow_map, v_shadow_coord + vec4(vec2(x, y) * texelSize * v_shadow_coord.w, 0.0, 0.0));
        }
    }
    shadow /= 9.0;
    
    // Se estiver fora do frustum da luz, não há sombra
    if(v_shadow_coord.z > 1.0) shadow = 1.0;

    // 4. Combina Luz Ambiente + Difusa * Sombra
    vec3 ambient = vec3(0.3);
    vec3 diffuse = vec3(0.7) * diff * shadow;
    
    vec3 final_color = color.rgb * (ambient + diffuse);
    
    f_color = vec4(final_color, color.a);
}
