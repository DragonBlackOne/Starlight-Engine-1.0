// Este projeto é feito por IA e só o prompt é feito por um humano.
#version 450 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float threshold = 0.8;

void main()
{            
    vec3 color = texture(screenTexture, TexCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > threshold)
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
