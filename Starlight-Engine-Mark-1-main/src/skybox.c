// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "skybox.h"
#include "shader.h"
#include "stb_image.h"
#include <stdio.h>

static float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

bool skybox_init(Skybox* skybox, const char* faces[6]) {
    // Carregar Cubemap
    glGenTextures(1, &skybox->texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {
            printf("[SKYBOX] Cubemap texture failed to load at path: %s\n", faces[i]);
            stbi_image_free(data);
            return false;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // VAO/VBO
    glGenVertexArrays(1, &skybox->vao);
    glGenBuffers(1, &skybox->vbo);
    glBindVertexArray(skybox->vao);
    glBindBuffer(GL_ARRAY_BUFFER, skybox->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Shader
    skybox->shader_program = shader_load_program("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    printf("[SKYBOX] Cubemap inicializado com sucesso.\n");
    return true;
}

void skybox_draw(Skybox* skybox, mat4 projection, mat4 view) {
    glDepthFunc(GL_LEQUAL);  // Mudar func de depth para passar quando for igual (pos.xyww)
    glUseProgram(skybox->shader_program);

    // Remover translacao da matriz view para o skybox nao se mover
    mat4 sky_view;
    glm_mat4_copy(view, sky_view);
    sky_view[3][0] = 0.0f;
    sky_view[3][1] = 0.0f;
    sky_view[3][2] = 0.0f;

    glUniformMatrix4fv(glGetUniformLocation(skybox->shader_program, "view"), 1, GL_FALSE, (float*)sky_view);
    glUniformMatrix4fv(glGetUniformLocation(skybox->shader_program, "projection"), 1, GL_FALSE, (float*)projection);

    glBindVertexArray(skybox->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // Resetar para o padrao
}

void skybox_destroy(Skybox* skybox) {
    glDeleteVertexArrays(1, &skybox->vao);
    glDeleteBuffers(1, &skybox->vbo);
    glDeleteTextures(1, &skybox->texture_id);
    glDeleteProgram(skybox->shader_program);
}
