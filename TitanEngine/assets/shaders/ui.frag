// Este projeto é feito por IA e só o prompt é feito por um humano.
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        float alpha = texture(uTexture, TexCoords).r;
        if (alpha < 0.05) discard;
        FragColor = vec4(uColor.rgb, uColor.a * alpha);
    } else {
        FragColor = uColor;
    }
}
