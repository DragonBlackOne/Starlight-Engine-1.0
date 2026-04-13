#include "shadow.h"
#include "shader.h"
#include <stdio.h>
#include <math.h>

bool shadow_init(ShadowMap* shadow, int resolution, int num_cascades) {
    if (num_cascades > MAX_CASCADES) num_cascades = MAX_CASCADES;
    shadow->resolution = resolution;
    shadow->num_cascades = num_cascades;

    glGenFramebuffers(1, &shadow->fbo);
    
    // Criar Texture Array para as cascatas
    glGenTextures(1, &shadow->depth_texture_array);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow->depth_texture_array);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, resolution, resolution, num_cascades, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow->fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow->depth_texture_array, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("[SHADOW] FBO de Array de Sombras incompleto!\n");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shadow->shader_program = shader_load_program("assets/shaders/shadow_depth.vert", "assets/shaders/shadow_depth.frag");
    
    return true;
}

void shadow_calculate_cascades(ShadowMap* shadow, mat4 view, mat4 projection, vec3 light_dir, float near, float far) {
    // Definir as divisoes das cascatas (exponencial para mais detalhe perto)
    shadow->cascade_splits[0] = far / 50.0f;
    shadow->cascade_splits[1] = far / 25.0f;
    shadow->cascade_splits[2] = far / 10.0f;
    shadow->cascade_splits[3] = far;

    mat4 inv_view_proj;
    mat4 view_proj;
    glm_mat4_mul(projection, view, view_proj);
    glm_mat4_inv(view_proj, inv_view_proj);

    for (int cascade = 0; cascade < shadow->num_cascades; ++cascade) {
        float prev_split = (cascade == 0) ? near : shadow->cascade_splits[cascade - 1];
        float next_split = shadow->cascade_splits[cascade];

        // Calcular os 8 cantos do frustum para esta fatia
        vec4 frustum_corners[8] = {
            {-1,  1, -1, 1}, { 1,  1, -1, 1}, { 1, -1, -1, 1}, {-1, -1, -1, 1},
            {-1,  1,  1, 1}, { 1,  1,  1, 1}, { 1, -1,  1, 1}, {-1, -1,  1, 1}
        };

        for (int i = 0; i < 8; ++i) {
            vec4 pt;
            glm_mat4_mulv(inv_view_proj, frustum_corners[i], pt);
            glm_vec4_scale(pt, 1.0f / pt[3], pt);
            glm_vec4_copy(pt, frustum_corners[i]);
        }

        // Centro do frustum
        vec3 center = {0,0,0};
        for (int i = 0; i < 8; ++i) glm_vec3_add(center, frustum_corners[i], center);
        glm_vec3_scale(center, 1.0f / 8.0f, center);

        // Matriz da luz (view)
        mat4 light_view;
        vec3 up = {0, 1, 0};
        vec3 light_pos;
        glm_vec3_sub(center, light_dir, light_pos);
        glm_lookat(light_pos, center, up, light_view);

        // Encontrar limites no espaco da luz
        float min_x = 1e6, max_x = -1e6;
        float min_y = 1e6, max_y = -1e6;
        float min_z = 1e6, max_z = -1e6;

        for (int i = 0; i < 8; ++i) {
            vec4 pt_light;
            glm_mat4_mulv(light_view, frustum_corners[i], pt_light);
            min_x = fminf(min_x, pt_light[0]); max_x = fmaxf(max_x, pt_light[0]);
            min_y = fminf(min_y, pt_light[1]); max_y = fmaxf(max_y, pt_light[1]);
            min_z = fminf(min_z, pt_light[2]); max_z = fmaxf(max_z, pt_light[2]);
        }

        // Estender Z para capturar objetos fora do frustum que projetam sombra
        float z_mult = 10.0f;
        min_z = (min_z < 0) ? min_z * z_mult : min_z / z_mult;
        max_z = (max_z < 0) ? max_z / z_mult : max_z * z_mult;

        mat4 light_projection;
        glm_ortho(min_x, max_x, min_y, max_y, min_z, max_z, light_projection);
        glm_mat4_mul(light_projection, light_view, shadow->light_space_matrices[cascade]);
    }
}

void shadow_begin_pass(ShadowMap* shadow, int cascade_index) {
    glViewport(0, 0, shadow->resolution, shadow->resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow->fbo);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow->depth_texture_array, 0, cascade_index);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadow->shader_program);
    glUniformMatrix4fv(glGetUniformLocation(shadow->shader_program, "lightSpaceMatrix"), 1, GL_FALSE, (float*)shadow->light_space_matrices[cascade_index]);
}

void shadow_end_pass(int screen_width, int screen_height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screen_width, screen_height);
}

void shadow_bind_texture(ShadowMap* shadow, int texture_unit) {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow->depth_texture_array);
}

void shadow_destroy(ShadowMap* shadow) {
    glDeleteFramebuffers(1, &shadow->fbo);
    glDeleteTextures(1, &shadow->depth_texture_array);
    glDeleteProgram(shadow->shader_program);
}
