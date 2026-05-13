#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec2 iPos;
layout (location = 3) in vec2 iSize;
layout (location = 4) in vec4 iColor;

out vec2 TexCoords;
out vec4 vColor;

uniform mat4 projection;
uniform vec2 position;
uniform vec2 size;
uniform vec4 color;
uniform int use_instancing;
uniform float rotation;

void main()
{
    vec2 final_pos;
    if (use_instancing == 1) {
        final_pos = iPos + (aPos * iSize);
        vColor = iColor;
    } else {
        // Apply rotation
        float c = cos(rotation);
        float s = sin(rotation);
        mat2 rot_mat = mat2(c, -s, s, c);
        vec2 rotated_pos = rot_mat * (aPos * size);
        final_pos = position + rotated_pos;
        vColor = color;
    }
    gl_Position = projection * vec4(final_pos, 0.0, 1.0); 
    TexCoords = aTexCoords;
}
