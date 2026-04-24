// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef GIZMO_H
#define GIZMO_H

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    GLuint vao, vbo;
    GLuint shader;
} GizmoRenderer;

void gizmo_init(GizmoRenderer* gr);
void gizmo_draw_line(GizmoRenderer* gr, vec3 start, vec3 end, vec3 color, mat4 view, mat4 proj);
void gizmo_draw_cube(GizmoRenderer* gr, vec3 pos, vec3 size, vec3 color, mat4 view, mat4 proj);
void gizmo_destroy(GizmoRenderer* gr);

#endif
