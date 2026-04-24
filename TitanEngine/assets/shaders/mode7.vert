// Este projeto é feito por IA e só o prompt é feito por um humano.
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec2 TexCoords;

void main() {
    TexCoords = aTex;
    gl_Position = vec4(aPos, 1.0); // Full screen quad
}
