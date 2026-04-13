#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 position;
uniform vec2 size;
uniform float rotation; // in radians
uniform vec4 uv_rect; // (sx, sy, sw, sh)

void main()
{
    // Local transform around center (0,0 is top-left in aPos)
    // aPos is 0.0 to 1.0. We shift it to -0.5 to 0.5 for rotation.
    vec2 half_size = size * 0.5;
    vec2 local_pos = (aPos * size) - half_size;
    
    // Rotate
    float c = cos(rotation);
    float s = sin(rotation);
    vec2 rotated_pos = vec2(
        local_pos.x * c - local_pos.y * s,
        local_pos.x * s + local_pos.y * c
    );
    
    // Move back + add position overlay
    vec2 final_pos = position + half_size + rotated_pos;
    
    gl_Position = projection * vec4(final_pos, 0.0, 1.0); 
    
    // Sub-texture mapping
    TexCoords = vec2(uv_rect.x + (aTexCoords.x * uv_rect.z),
                     uv_rect.y + (aTexCoords.y * uv_rect.w));
}
