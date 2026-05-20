#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    // Apply a small half-pixel offset to prevent blocky sampling on some GPUs
    vec4 world_pos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * world_pos;
    
    // Half-pixel offset for UVs to sharpen text
    TexCoords = aTexCoords;
}
