#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform int uUseTexture;

void main()
{
    if (uUseTexture == 1) {
        // For GL_RED font textures, use R channel as alpha mask
        float mask = texture(uTexture, TexCoords).r;
        FragColor = vec4(uColor.rgb, uColor.a * mask);
        
        // If it's too dark, discard
        if (FragColor.a < 0.1) discard;
    } else {
        FragColor = uColor;
    }
}
