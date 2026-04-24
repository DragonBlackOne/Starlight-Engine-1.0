#version 330

in vec4 v_color;
in float v_life;

out vec4 f_color;

void main() {
    if (v_life <= 0.0) discard;
    
    // Circular point sprite
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist_sq = dot(coord, coord);
    
    if (dist_sq > 0.25) discard;
    
    // Soft edge
    float alpha = 1.0 - smoothstep(0.2, 0.25, dist_sq);
    
    f_color = v_color * alpha;
    f_color.a *= 0.8; // Base transparency
}
