// Este projeto é feito por IA e só o prompt é feito por um humano.
#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D ssaoTexture;
uniform float uTime;

void main()
{
    // Subtle CRT distortion
    vec2 pos = TexCoords - 0.5;
    float dist = dot(pos, pos);
    vec2 curved_texCoords = TexCoords + pos * (dist * 0.2);

    if(curved_texCoords.x < 0.0 || curved_texCoords.x > 1.0 || curved_texCoords.y < 0.0 || curved_texCoords.y > 1.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 color = texture(screenTexture, curved_texCoords).rgb;
    float ao = texture(ssaoTexture, curved_texCoords).r;
    color *= ao;

    // Scanlines
    float scanline = sin(curved_texCoords.y * 800.0 * 3.14159) * 0.04;
    color -= scanline;

    // Subtle RGB Shift
    float r = texture(screenTexture, curved_texCoords + vec2(0.002, 0.0)).r;
    float b = texture(screenTexture, curved_texCoords - vec2(0.002, 0.0)).b;
    color.r = mix(color.r, r, 0.5);
    color.b = mix(color.b, b, 0.5);

    // Vignette
    float vignette = 1.0 - smoothstep(0.4, 1.0, length(pos));
    color *= vignette;

    FragColor = vec4(color, 1.0);
}
