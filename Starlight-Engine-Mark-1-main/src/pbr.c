#include "pbr.h"
#include "shader.h"
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

// Cube definition
static float cube_vertices[] = {
    // back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
    // front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    // right face
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
     1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
};
static unsigned int cubeVAO = 0, cubeVBO = 0;
static void render_cube() {
    if (cubeVAO == 0) {
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

static float quadVertices[] = {
    // positions        // texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};
static unsigned int quadVAO = 0, quadVBO = 0;
static void render_quad() {
    if (quadVAO == 0) {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

bool pbr_init_ibl(const char* hdr_filepath, PBR_IBL* out_ibl) {
    if (!hdr_filepath || !out_ibl) return false;

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(hdr_filepath, &width, &height, &nrComponents, 0);
    if (!data) {
        printf("[PBR] Failed to load HDR image: %s\n", hdr_filepath);
        return false;
    }

    GLuint hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    GLuint fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glGenTextures(1, &out_ibl->env_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_ibl->env_cubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mat4 captureProjection;
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, 10.0f, captureProjection);
    mat4 captureViews[6];
    vec3 center = {0,0,0};
    vec3 dirs[6] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}};
    vec3 ups[6]  = {{0,-1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}, {0,-1,0}, {0,-1,0}};
    
    for (int i=0; i<6; i++) {
        vec3 target;
        glm_vec3_add(center, dirs[i], target);
        glm_lookat(center, target, ups[i], captureViews[i]);
    }

    // 1. Convert HDR equirectangular environment map to cubemap
    GLuint equirect_shader = shader_load_program("assets/shaders/cubemap.vert", "assets/shaders/equirect_to_cube.frag");
    glUseProgram(equirect_shader);
    glUniform1i(glGetUniformLocation(equirect_shader, "equirectangularMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(equirect_shader, "projection"), 1, GL_FALSE, (float*)captureProjection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (unsigned int i = 0; i < 6; ++i) {
        glUniformMatrix4fv(glGetUniformLocation(equirect_shader, "view"), 1, GL_FALSE, (float*)captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, out_ibl->env_cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_cube();
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_ibl->env_cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // 2. Irradiance Map
    glGenTextures(1, &out_ibl->irradiance_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_ibl->irradiance_map);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    GLuint irradiance_shader = shader_load_program("assets/shaders/cubemap.vert", "assets/shaders/irradiance.frag");
    glUseProgram(irradiance_shader);
    glUniform1i(glGetUniformLocation(irradiance_shader, "environmentMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(irradiance_shader, "projection"), 1, GL_FALSE, (float*)captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_ibl->env_cubemap);

    glViewport(0, 0, 32, 32);
    for (unsigned int i = 0; i < 6; ++i) {
        glUniformMatrix4fv(glGetUniformLocation(irradiance_shader, "view"), 1, GL_FALSE, (float*)captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, out_ibl->irradiance_map, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_cube();
    }

    // 3. Prefilter Map
    glGenTextures(1, &out_ibl->prefilter_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_ibl->prefilter_map);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    GLuint prefilter_shader = shader_load_program("assets/shaders/cubemap.vert", "assets/shaders/prefilter.frag");
    glUseProgram(prefilter_shader);
    glUniform1i(glGetUniformLocation(prefilter_shader, "environmentMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(prefilter_shader, "projection"), 1, GL_FALSE, (float*)captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, out_ibl->env_cubemap);

    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        unsigned int mipWidth  = (unsigned int)(128 * pow(0.5, mip));
        unsigned int mipHeight = (unsigned int)(128 * pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        glUniform1f(glGetUniformLocation(prefilter_shader, "roughness"), roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            glUniformMatrix4fv(glGetUniformLocation(prefilter_shader, "view"), 1, GL_FALSE, (float*)captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, out_ibl->prefilter_map, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render_cube();
        }
    }

    // 4. BRDF LUT
    glGenTextures(1, &out_ibl->brdf_lut);
    glBindTexture(GL_TEXTURE_2D, out_ibl->brdf_lut);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_ibl->brdf_lut, 0);

    glViewport(0, 0, 512, 512);
    GLuint brdf_shader = shader_load_program("assets/shaders/screen_quad.vert", "assets/shaders/brdf.frag");
    glUseProgram(brdf_shader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_quad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Cleanup
    glDeleteProgram(equirect_shader);
    glDeleteProgram(irradiance_shader);
    glDeleteProgram(prefilter_shader);
    glDeleteProgram(brdf_shader);
    glDeleteTextures(1, &hdrTexture);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);

    printf("[PBR] IBL Pipeline successfully generated from %s\n", hdr_filepath);
    return true;
}

void pbr_destroy_ibl(PBR_IBL* ibl) {
    glDeleteTextures(1, &ibl->env_cubemap);
    glDeleteTextures(1, &ibl->irradiance_map);
    glDeleteTextures(1, &ibl->prefilter_map);
    glDeleteTextures(1, &ibl->brdf_lut);
}
