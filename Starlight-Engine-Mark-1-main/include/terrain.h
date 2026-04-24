// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    GLuint vao, vbo, ebo;
    int index_count;
} Terrain;

// Gera um terreno de (width x depth) com espacamento (scale)
void terrain_init(Terrain* terrain, int width, int depth, float scale);
void terrain_draw(Terrain* terrain, GLuint shader_program);
void terrain_destroy(Terrain* terrain);

#endif
