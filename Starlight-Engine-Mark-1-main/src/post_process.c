#include "post_process.h"
#include "shader.h"
#include <stdio.h>

bool post_process_init(PostProcess* pp, int render_w, int render_h, int win_w, int win_h) {
    pp->width = render_w;
    pp->height = render_h;
    pp->window_width = win_w;
    pp->window_height = win_h;

    // Se a resolução interna for menor que a da janela (Retro Mode), aplicar pixel perfect scaling
    int filter = (render_w < win_w) ? GL_NEAREST : GL_LINEAR;

    // Criar FBO principal (HDR)
    glGenFramebuffers(1, &pp->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pp->fbo);

    glGenTextures(1, &pp->texture_id);
    glBindTexture(GL_TEXTURE_2D, pp->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, render_w, render_h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    // Para upscale puro, a magnificação é obrigatoriamente GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp->texture_id, 0);

    glGenTextures(1, &pp->depth_id);
    glBindTexture(GL_TEXTURE_2D, pp->depth_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, render_w, render_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pp->depth_id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("[POST] FBO principal incompleto!\n");
        return false;
    }

    // Ping-pong FBOs para blur (Bloom)
    glGenFramebuffers(2, pp->pingpongFBO);
    glGenTextures(2, pp->pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pp->pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pp->pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, render_w, render_h, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp->pingpongColorbuffers[i], 0);
    }

    // Shaders (Carregar do caminho root-relative)
    pp->post_shader   = shader_load_program("assets/shaders/quad.vert", "assets/shaders/post_process.frag");
    if (!pp->post_shader) printf("[POST] Falha ao carregar post_process.frag\n");
    
    pp->blur_shader   = shader_load_program("assets/shaders/quad.vert", "assets/shaders/blur.frag");
    if (!pp->blur_shader) printf("[POST] Falha ao carregar blur.frag\n");
    
    pp->bright_shader = shader_load_program("assets/shaders/quad.vert", "assets/shaders/bright.frag");
    if (!pp->bright_shader) printf("[POST] Falha ao carregar bright.frag\n");

    // Quad VAO (para renderizar na tela)
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &pp->quadVAO);
    glGenBuffers(1, &pp->quadVBO);
    glBindVertexArray(pp->quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pp->quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void post_process_begin(PostProcess* pp) {
    glBindFramebuffer(GL_FRAMEBUFFER, pp->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void post_process_end(PostProcess* pp) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void post_process_render(PostProcess* pp, float exposure, mat4 view, mat4 proj) {
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, pp->width, pp->height);

    // 1. Bright Extraction
    glBindFramebuffer(GL_FRAMEBUFFER, pp->pingpongFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT); // Clear temp buffer
    glUseProgram(pp->bright_shader);
    glUniform1f(glGetUniformLocation(pp->bright_shader, "threshold"), 0.5f); 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pp->texture_id);
    glBindVertexArray(pp->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 2. Blur Gaussiano
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 6;
    glUseProgram(pp->blur_shader);
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pp->pingpongFBO[horizontal]);
        glUniform1i(glGetUniformLocation(pp->blur_shader, "horizontal"), horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? pp->pingpongColorbuffers[0] : pp->pingpongColorbuffers[!horizontal]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    // 3. Final Combine + PostFX (SSR)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, pp->window_width, pp->window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(pp->post_shader);
    
    // Matrizes invertidas para raymarching no Screen Space
    mat4 inv_view, inv_proj;
    glm_mat4_inv(view, inv_view);
    glm_mat4_inv(proj, inv_proj);

    glUniformMatrix4fv(glGetUniformLocation(pp->post_shader, "u_inv_view"), 1, GL_FALSE, (float*)inv_view);
    glUniformMatrix4fv(glGetUniformLocation(pp->post_shader, "u_inv_proj"), 1, GL_FALSE, (float*)inv_proj);
    glUniformMatrix4fv(glGetUniformLocation(pp->post_shader, "u_view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(pp->post_shader, "u_proj"), 1, GL_FALSE, (float*)proj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pp->texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pp->pingpongColorbuffers[!horizontal]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, pp->depth_id);
    
    glUniform1i(glGetUniformLocation(pp->post_shader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(pp->post_shader, "bloomBlur"), 1);
    glUniform1i(glGetUniformLocation(pp->post_shader, "depthTexture"), 2);
    glUniform1f(glGetUniformLocation(pp->post_shader, "exposure"), exposure);
    glUniform1i(glGetUniformLocation(pp->post_shader, "u_retro_mode"), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST); // Re-enable for next frame
}

void post_process_destroy(PostProcess* pp) {
    glDeleteFramebuffers(1, &pp->fbo);
    glDeleteTextures(1, &pp->texture_id);
    glDeleteTextures(1, &pp->depth_id);
    glDeleteFramebuffers(2, pp->pingpongFBO);
    glDeleteTextures(2, pp->pingpongColorbuffers);
    
    shader_delete(pp->post_shader);
    shader_delete(pp->blur_shader);
    shader_delete(pp->bright_shader);
}
