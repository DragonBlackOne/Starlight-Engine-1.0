// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "ssao.h"
#include "shader.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

bool ssao_init(SSAO* ssao, int width, int height) {
    // 1. Gerar Kernel de Amostragem (64 samples)
    for (unsigned int i = 0; i < 64; ++i) {
        vec3 sample = {
            ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f,
            ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f,
            ((float)rand() / (float)RAND_MAX)
        };
        glm_vec3_normalize(sample);
        float scale = (float)i / 64.0f;
        scale = lerp(0.1f, 1.0f, scale * scale);
        glm_vec3_scale(sample, scale, ssao->ssao_kernel[i]);
    }

    // 2. Gerar Ruido de Rotacao (4x4)
    vec3 ssaoNoise[16];
    for (unsigned int i = 0; i < 16; i++) {
        ssaoNoise[i][0] = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        ssaoNoise[i][1] = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        ssaoNoise[i][2] = 0.0f;
    }
    glGenTextures(1, &ssao->noise_texture);
    glBindTexture(GL_TEXTURE_2D, ssao->noise_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 3. FBO para o SSAO Principal
    glGenFramebuffers(1, &ssao->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao->fbo);
    glGenTextures(1, &ssao->ssao_color_buffer);
    glBindTexture(GL_TEXTURE_2D, ssao->ssao_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao->ssao_color_buffer, 0);

    // 4. FBO para o SSAO Blur (evita ruido)
    glGenFramebuffers(1, &ssao->blur_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao->blur_fbo);
    glGenTextures(1, &ssao->blur_color_buffer);
    glBindTexture(GL_TEXTURE_2D, ssao->blur_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao->blur_color_buffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("[SSAO] FBO incompleto!\n");
        return false;
    }

    ssao->ssao_shader = shader_load_program("assets/shaders/screen_quad.vert", "assets/shaders/ssao.frag");
    ssao->blur_shader = shader_load_program("assets/shaders/screen_quad.vert", "assets/shaders/ssao_blur.frag");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ssao_render(SSAO* ssao, GLuint depth_texture, mat4 projection, mat4 view, int width, int height) {
    // 1. Passe SSAO
    glBindFramebuffer(GL_FRAMEBUFFER, ssao->fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(ssao->ssao_shader);

    glUniform3fv(glGetUniformLocation(ssao->ssao_shader, "samples"), 64, (float*)ssao->ssao_kernel);
    glUniformMatrix4fv(glGetUniformLocation(ssao->ssao_shader, "projection"), 1, GL_FALSE, (float*)projection);
    glUniformMatrix4fv(glGetUniformLocation(ssao->ssao_shader, "view"), 1, GL_FALSE, (float*)view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glUniform1i(glGetUniformLocation(ssao->ssao_shader, "gDepth"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssao->noise_texture);
    glUniform1i(glGetUniformLocation(ssao->ssao_shader, "texNoise"), 1);

    // Render fullscreen quad (assume VAO bound from engine or use custom here)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 2. Passe de Blur (Suavizar ruido)
    glBindFramebuffer(GL_FRAMEBUFFER, ssao->blur_fbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(ssao->blur_shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssao->ssao_color_buffer);
    glUniform1i(glGetUniformLocation(ssao->blur_shader, "ssaoInput"), 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ssao_destroy(SSAO* ssao) {
    glDeleteFramebuffers(1, &ssao->fbo);
    glDeleteFramebuffers(1, &ssao->blur_fbo);
    glDeleteTextures(1, &ssao->ssao_color_buffer);
    glDeleteTextures(1, &ssao->blur_color_buffer);
    glDeleteTextures(1, &ssao->noise_texture);
}
