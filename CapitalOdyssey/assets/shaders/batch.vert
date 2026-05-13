#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aTextureIndex;
layout (location = 4) in float aFlags;

out vec2 TexCoords;
out vec4 VertexColor;
out float TextureIndex;
out float vFlags;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0); 
    TexCoords = aTexCoords;
    VertexColor = aColor;
    TextureIndex = aTextureIndex;
    vFlags = aFlags;
}
