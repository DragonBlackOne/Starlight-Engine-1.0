#version 410 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 VertexColor;
in float TextureIndex;
in float vFlags;

uniform sampler2D uTextures[16];

void main()
{
    vec4 texColor = VertexColor;
    int index = int(TextureIndex);

    // Sample texture
    vec4 sampleColor = vec4(1.0);
    switch(index)
    {
        case 0: sampleColor = texture(uTextures[0], TexCoords); break;
        case 1: sampleColor = texture(uTextures[1], TexCoords); break;
        case 2: sampleColor = texture(uTextures[2], TexCoords); break;
        case 3: sampleColor = texture(uTextures[3], TexCoords); break;
        case 4: sampleColor = texture(uTextures[4], TexCoords); break;
        case 5: sampleColor = texture(uTextures[5], TexCoords); break;
        case 6: sampleColor = texture(uTextures[6], TexCoords); break;
        case 7: sampleColor = texture(uTextures[7], TexCoords); break;
        case 8: sampleColor = texture(uTextures[8], TexCoords); break;
        case 9: sampleColor = texture(uTextures[9], TexCoords); break;
        case 10: sampleColor = texture(uTextures[10], TexCoords); break;
        case 11: sampleColor = texture(uTextures[11], TexCoords); break;
        case 12: sampleColor = texture(uTextures[12], TexCoords); break;
        case 13: sampleColor = texture(uTextures[13], TexCoords); break;
        case 14: sampleColor = texture(uTextures[14], TexCoords); break;
        case 15: sampleColor = texture(uTextures[15], TexCoords); break;
    }
    
    texColor *= sampleColor;

    // Isometric Tile Masking (Procedural Diamond)
    if (vFlags > 0.5 && vFlags < 1.5) {
        vec2 centeredUV = TexCoords - 0.5;
        if (abs(centeredUV.x) + abs(centeredUV.y * 2.0) > 0.5) {
            discard;
        }
    }
    
    // Anti-Checkerboard Filter (Removes typical transparency placeholders)
    if (vFlags > 1.5) {
        vec3 c = sampleColor.rgb;
        bool isGray = (abs(c.r - 0.8) < 0.05 && abs(c.g - 0.8) < 0.05 && abs(c.b - 0.8) < 0.05);
        bool isWhite = (c.r > 0.95 && c.g > 0.95 && c.b > 0.95);
        if (isGray || isWhite) discard;
    }

    if (texColor.a < 0.1) discard;
    FragColor = texColor;
}
