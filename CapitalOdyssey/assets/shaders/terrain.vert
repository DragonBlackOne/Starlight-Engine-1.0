
#version 330 core
in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;

out vec3 v_normal;
out vec3 v_frag_pos;
out vec2 v_uv;
out float v_height;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

// Simple noise for displacement
float hash(float n) { return fract(sin(n) * 753.5453123); }
float snoise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
                   mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
               mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
                   mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

void main() {
    float h = snoise(in_position * 1.5) * 0.4;
    h += snoise(in_position * 5.0) * 0.1;
    v_height = h;

    vec3 displaced = in_position + normalize(in_position) * h;
    
    vec4 world_pos = u_model * vec4(displaced, 1.0);
    gl_Position = u_projection * u_view * world_pos;
    
    // Logarithmic Depth Buffer
    float C = 1.0;
    float far = 100000.0;
    gl_Position.z = (2.0 * log(C * gl_Position.w + 1.0) / log(C * far + 1.0) - 1.0) * gl_Position.w;
    
    // Pass to frag
    v_frag_pos = world_pos.xyz;
    v_normal = mat3(u_model) * in_normal;
    v_uv = in_uv;
}
