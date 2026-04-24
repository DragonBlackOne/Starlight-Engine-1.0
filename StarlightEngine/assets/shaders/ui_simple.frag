#version 410 core
out vec4 FragColor;
in vec2 TexCoords;
in vec4 vColor;

uniform sampler2D tex;
uniform int use_texture;
uniform vec4 uv_rect; // x, y, w, h
uniform int is_circle;

void main()
{
    if (is_circle == 1) {
        float d = length(TexCoords - vec2(0.5, 0.5));
        if (d > 0.5) discard;
    }

    if (use_texture == 1) {
        vec2 uv = uv_rect.xy + (TexCoords * uv_rect.zw);
        FragColor = texture(tex, uv) * vColor;
    } else {
        FragColor = vColor;
    }

    if (FragColor.a < 0.1) discard;
}
