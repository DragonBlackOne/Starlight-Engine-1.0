#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform int uUseTexture;

void main()
{
    if (uUseTexture == 1) {
        // Red channel is used as the alpha mask for fonts
        float alpha = texture(uTexture, TexCoords).r;
        FragColor = vec4(uColor.rgb, uColor.a * alpha);
    } else {
        FragColor = uColor;
    }
    
    if (FragColor.a < 0.1) discard;
}
