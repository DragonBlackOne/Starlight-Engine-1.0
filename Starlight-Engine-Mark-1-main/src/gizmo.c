// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "gizmo.h"
#include "shader.h"
#include <stdio.h>

void gizmo_init(GizmoRenderer* gr) {
    glGenVertexArrays(1, &gr->vao);
    glGenBuffers(1, &gr->vbo);
    
    // Shader minimalista para Gizmos (cor solida)
    const char* vert = "#version 410 core\n"
                       "layout(location=0) in vec3 pos;\n"
                       "uniform mat4 mvp;\n"
                       "void main() { gl_Position = mvp * vec4(pos, 1.0); }";
    const char* frag = "#version 410 core\n"
                       "out vec4 color;\n"
                       "uniform vec3 gizmoColor;\n"
                       "void main() { color = vec4(gizmoColor, 1.0); }";

    // Nota: Aqui poderiamos salvar num arquivo, mas embutimos para o Editor ser auto-contido
    // Para simplificar, assumimos que shader_load_source existiria ou criamos inline
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert, NULL); glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag, NULL); glCompileShader(fs);
    gr->shader = glCreateProgram();
    glAttachShader(gr->shader, vs); glAttachShader(gr->shader, fs);
    glLinkProgram(gr->shader);
}

void gizmo_draw_line(GizmoRenderer* gr, vec3 start, vec3 end, vec3 color, mat4 view, mat4 proj) {
    float verts[] = { start[0], start[1], start[2], end[0], end[1], end[2] };
    
    mat4 mvp;
    glm_mat4_mul(proj, view, mvp);

    glUseProgram(gr->shader);
    glUniformMatrix4fv(glGetUniformLocation(gr->shader, "mvp"), 1, GL_FALSE, (float*)mvp);
    glUniform3fv(glGetUniformLocation(gr->shader, "gizmoColor"), 1, (float*)color);

    glBindVertexArray(gr->vao);
    glBindBuffer(GL_ARRAY_BUFFER, gr->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*4, (void*)0);

    glDrawArrays(GL_LINES, 0, 2);
}

void gizmo_draw_cube(GizmoRenderer* gr, vec3 pos, vec3 size, vec3 color, mat4 view, mat4 proj) {
    float x1 = pos[0] - size[0], x2 = pos[0] + size[0];
    float y1 = pos[1] - size[1], y2 = pos[1] + size[1];
    float z1 = pos[2] - size[2], z2 = pos[2] + size[2];

    vec3 v[8] = {
        {x1, y1, z1}, {x2, y1, z1}, {x2, y2, z1}, {x1, y2, z1},
        {x1, y1, z2}, {x2, y1, z2}, {x2, y2, z2}, {x1, y2, z2}
    };

    // Bottom
    gizmo_draw_line(gr, v[0], v[1], color, view, proj);
    gizmo_draw_line(gr, v[1], v[2], color, view, proj);
    gizmo_draw_line(gr, v[2], v[3], color, view, proj);
    gizmo_draw_line(gr, v[3], v[0], color, view, proj);

    // Top
    gizmo_draw_line(gr, v[4], v[5], color, view, proj);
    gizmo_draw_line(gr, v[5], v[6], color, view, proj);
    gizmo_draw_line(gr, v[6], v[7], color, view, proj);
    gizmo_draw_line(gr, v[7], v[4], color, view, proj);

    // Vertical
    gizmo_draw_line(gr, v[0], v[4], color, view, proj);
    gizmo_draw_line(gr, v[1], v[5], color, view, proj);
    gizmo_draw_line(gr, v[2], v[6], color, view, proj);
    gizmo_draw_line(gr, v[3], v[7], color, view, proj);
}

void gizmo_destroy(GizmoRenderer* gr) {
    glDeleteVertexArrays(1, &gr->vao);
    glDeleteBuffers(1, &gr->vbo);
    glDeleteProgram(gr->shader);
}
