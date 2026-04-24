// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "pbr.h"
#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTANCES_PER_BATCH 1024

typedef struct {
    unsigned int vao;
    unsigned int index_count;
    GLuint instance_vbo;
    mat4 instance_matrices[MAX_INSTANCES_PER_BATCH];
    int instance_count;
    GLuint shader;
} RenderBatch;

void batch_renderer_init(RenderBatch* batch, unsigned int vao, unsigned int index_count, GLuint shader) {
    batch->vao = vao;
    batch->index_count = index_count;
    batch->shader = shader;
    batch->instance_count = 0;

    glBindVertexArray(vao);
    glGenBuffers(1, &batch->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, batch->instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES_PER_BATCH * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);

    // Configurar atributos da matriz de instancia (atrib 6, 7, 8, 9 no shader pbr.vert)
    // Nota: Matrizes ocupam 4 slots de atributos
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(6 + i);
        glVertexAttribPointer(6 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
        glVertexAttribDivisor(6 + i, 1); // Atualiza por instancia, nao por vertice
    }
    glBindVertexArray(0);
}

void batch_renderer_add(RenderBatch* batch, mat4 model) {
    if (batch->instance_count >= MAX_INSTANCES_PER_BATCH) return;
    glm_mat4_copy(model, batch->instance_matrices[batch->instance_count]);
    batch->instance_count++;
}

void batch_renderer_flush(RenderBatch* batch) {
    if (batch->instance_count == 0) return;

    glUseProgram(batch->shader);
    glUniform1i(glGetUniformLocation(batch->shader, "u_is_instanced"), 1);

    glBindBuffer(GL_ARRAY_BUFFER, batch->instance_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batch->instance_count * sizeof(mat4), batch->instance_matrices);

    glBindVertexArray(batch->vao);
    glDrawElementsInstanced(GL_TRIANGLES, batch->index_count, GL_UNSIGNED_INT, 0, batch->instance_count);
    
    batch->instance_count = 0;
    glUniform1i(glGetUniformLocation(batch->shader, "u_is_instanced"), 0);
    glBindVertexArray(0);
}
