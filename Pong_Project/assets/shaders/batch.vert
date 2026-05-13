#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aTexIndex;
layout (location = 4) in float aFlags;

out vec2 TexCoords;
out vec4 VertexColor;
out float TexIndex;
out float Flags;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0); 
    TexCoords = aTexCoords;
    VertexColor = aColor;
    TexIndex = aTexIndex;
    Flags = aFlags;
}
