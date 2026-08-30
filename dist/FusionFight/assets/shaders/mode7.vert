#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 projection;
uniform vec2 position;
uniform vec2 size;

out vec2 v_uv;

void main() {
    mat4 model = mat4(1.0);
    model[3][0] = position.x;
    model[3][1] = position.y;
    model[0][0] = size.x;
    model[1][1] = size.y;

    vec4 worldPos = model * vec4(aPos, 0.0, 1.0);
    gl_Position = projection * worldPos;
    v_uv = aPos; // Enviando de 0.0 a 1.0
}
