#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 color;
uniform sampler2D tex;
uniform int use_texture;
uniform int is_circle;

void main()
{
    if (is_circle == 1) {
        vec2 p = TexCoords - vec2(0.5);
        if (dot(p, p) > 0.25) {
            discard;
        }
    }

    if (use_texture == 1) {
        FragColor = texture(tex, TexCoords) * color;
    } else {
        FragColor = color;
    }
}
