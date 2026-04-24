#version 330 core

in vec3 v_normal;
in vec3 v_position;
in vec2 v_uv;

out vec4 f_color;

uniform sampler2D u_texture;
uniform vec3 u_light_dir;
uniform vec3 u_sky_color;

void main() {
    // Texture sampling
    vec4 texColor = texture(u_texture, v_uv);
    
    // Lighting
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_light_dir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float ambient = 0.4;
    
    vec3 lighting = (ambient + diff * 0.8) * u_sky_color;
    
    f_color = vec4(texColor.rgb * lighting, texColor.a);
}
