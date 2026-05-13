#version 410 core
out vec4 outColor;
in vec3 v_uv;

uniform samplerCube skybox;

void main() {
    outColor = texture(skybox, v_uv);
}
