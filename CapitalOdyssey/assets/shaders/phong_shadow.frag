#version 330 core
in vec3 v_normal;
in vec3 v_frag_pos;
in vec2 v_uv;
in vec4 v_frag_pos_light_space;
out vec4 f_color;
uniform vec3 u_light_pos;
uniform vec3 u_view_pos;
uniform vec4 u_color;
uniform sampler2D u_shadow_map;
float calculate_shadow() {
    vec3 proj_coords = v_frag_pos_light_space.xyz / v_frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    float closest_depth = texture(u_shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    float shadow = current_depth > closest_depth + 0.005 ? 1.0 : 0.0;
    return shadow;
}
void main() {
    vec3 light_color = vec3(1.0, 1.0, 1.0);
    vec3 norm = normalize(v_normal);
    vec3 light_dir = normalize(u_light_pos - v_frag_pos);
    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * light_color;
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;
    float specular_strength = 0.5;
    vec3 view_dir = normalize(u_view_pos - v_frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;
    float shadow = calculate_shadow();
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    f_color = vec4(lighting, 1.0) * u_color;
}
