#version 410 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 VertexColor;
in float TexIndex;
in float Flags;

uniform sampler2D uTextures[16];

void main()
{
    int index = int(TexIndex + 0.5);
    vec4 texColor = vec4(1.0);
    
    // Manual texture array sampling for compatibility
    if (index == 0) texColor = texture(uTextures[0], TexCoords);
    else if (index == 1) {
        float a = texture(uTextures[1], TexCoords).r;
        texColor = vec4(1.0, 1.0, 1.0, a);
    }
    else if (index == 2) texColor = texture(uTextures[2], TexCoords);
    else if (index == 3) texColor = texture(uTextures[3], TexCoords);
    else if (index == 4) texColor = texture(uTextures[4], TexCoords);
    else if (index == 5) texColor = texture(uTextures[5], TexCoords);
    else if (index == 6) texColor = texture(uTextures[6], TexCoords);
    else if (index == 7) texColor = texture(uTextures[7], TexCoords);
    else if (index == 8) texColor = texture(uTextures[8], TexCoords);
    else if (index == 9) texColor = texture(uTextures[9], TexCoords);
    else if (index == 10) texColor = texture(uTextures[10], TexCoords);
    else if (index == 11) texColor = texture(uTextures[11], TexCoords);
    else if (index == 12) texColor = texture(uTextures[12], TexCoords);
    else if (index == 13) texColor = texture(uTextures[13], TexCoords);
    else if (index == 14) texColor = texture(uTextures[14], TexCoords);
    else if (index == 15) texColor = texture(uTextures[15], TexCoords);

    FragColor = texColor * VertexColor;
}
