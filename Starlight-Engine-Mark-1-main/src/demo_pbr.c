#include "starlight.h"
#include "pbr.h"
#include "shadow.h"
#include "ssao.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Globals
static PBR_IBL g_ibl = {0};
static ShadowMap g_shadow = {0};
static SSAO g_ssao = {0};
static GLuint g_pbr_shader = 0;
static unsigned int sphereVAO = 0;
static unsigned int indexCount = 0;
static bool g_has_ibl = false;
static AudioSource* g_3d_sound = NULL;

static void render_sphere() {
    if (sphereVAO == 0) {
        glGenVertexArrays(1, &sphereVAO);
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const unsigned int maxVertices = (X_SEGMENTS + 1) * (Y_SEGMENTS + 1);
        
        float* positions = (float*)malloc(maxVertices * 3 * sizeof(float));
        float* uv = (float*)malloc(maxVertices * 2 * sizeof(float));
        float* normals = (float*)malloc(maxVertices * 3 * sizeof(float));
        unsigned int* indices = (unsigned int*)malloc(maxVertices * 6 * sizeof(unsigned int));

        int vCount = 0;
        int iCount = 0;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = cosf(xSegment * 2.0f * PI) * sinf(ySegment * PI);
                float yPos = cosf(ySegment * PI);
                float zPos = sinf(xSegment * 2.0f * PI) * sinf(ySegment * PI);

                positions[vCount * 3 + 0] = xPos;
                positions[vCount * 3 + 1] = yPos;
                positions[vCount * 3 + 2] = zPos;
                uv[vCount * 2 + 0] = xSegment;
                uv[vCount * 2 + 1] = ySegment;
                normals[vCount * 3 + 0] = xPos;
                normals[vCount * 3 + 1] = yPos;
                normals[vCount * 3 + 2] = zPos;
                vCount++;
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            if (!oddRow) {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                    indices[iCount++] = y * (X_SEGMENTS + 1) + x;
                    indices[iCount++] = (y + 1) * (X_SEGMENTS + 1) + x;
                }
            } else {
                for (int x = X_SEGMENTS; x >= 0; --x) {
                    indices[iCount++] = (y + 1) * (X_SEGMENTS + 1) + x;
                    indices[iCount++] = y * (X_SEGMENTS + 1) + x;
                }
            }
            oddRow = !oddRow;
        }
        indexCount = iCount;

        float* data = (float*)malloc(vCount * 8 * sizeof(float));
        for(int i = 0; i < vCount; ++i) {
            data[i*8 + 0] = positions[i*3 + 0];
            data[i*8 + 1] = positions[i*3 + 1];
            data[i*8 + 2] = positions[i*3 + 2];
            data[i*8 + 3] = normals[i*3 + 0];
            data[i*8 + 4] = normals[i*3 + 1];
            data[i*8 + 5] = normals[i*3 + 2];
            data[i*8 + 6] = uv[i*2 + 0];
            data[i*8 + 7] = uv[i*2 + 1];
        }

        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vCount * 8 * sizeof(float), data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, iCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        
        free(positions); free(uv); free(normals); free(indices); free(data);
    }
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

static void render_scene(GLuint shader, bool depth_only) {
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5f;
    for (int row = 0; row < nrRows; ++row) {
        if (!depth_only) glUniform1f(glGetUniformLocation(shader, "u_metallic"), (float)row / (float)nrRows);
        for (int col = 0; col < nrColumns; ++col) {
            if (!depth_only) glUniform1f(glGetUniformLocation(shader, "u_roughness"), fmaxf((float)col / (float)nrColumns, 0.05f));
            
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            vec3 pos = { (col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f };
            glm_translate(model, pos);
            glUniformMatrix4fv(glGetUniformLocation(shader, depth_only ? "model" : "u_model"), 1, GL_FALSE, (float*)model);
            if (!depth_only) {
                glUniform4f(glGetUniformLocation(shader, "u_base_color"), 1.0f, 0.2f, 0.2f, 1.0f);
                glUniform3f(glGetUniformLocation(shader, "u_emissive_factor"), 0.0f, 0.0f, 0.0f);
            }
            render_sphere();
        }
    }
}

static void OnStart(SLF_App* app) {
    printf(">>> STARLIGHT ENGINE MAX PBR + CSM + SSAO SHOWCASE <<<\n");
    camera_init(&app->camera, (vec3){0.0f, 5.0f, 15.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, -15.0f);
    
    g_pbr_shader = shader_load_program("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
    shadow_init(&g_shadow, 2048, 4);
    ssao_init(&g_ssao, app->width, app->height);
    g_has_ibl = pbr_init_ibl("assets/textures/environment.hdr", &g_ibl);

    glUseProgram(g_pbr_shader);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_irradiance_map"), 0);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_prefilter_map"), 1);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_brdf_lut"), 2);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_ssao_map"), 7);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_shadow_map"), 8);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_use_ibl"), g_has_ibl ? 1 : 0);
    glUniform1i(glGetUniformLocation(g_pbr_shader, "u_use_ssao"), 1);

    g_3d_sound = audio_create_source("assets/audio/background_music.mp3");
    if (g_3d_sound) audio_source_play(g_3d_sound, true, 0.5f);
}

static void OnUpdate(SLF_App* app) {
    if (app->keys[SDL_SCANCODE_ESCAPE]) app->running = false;
    audio_set_listener(app->camera.position, app->camera.front, app->camera.up, (vec3){0,0,0});
}

static void OnRender(SLF_App* app) {
    vec3 light_dir = {0.5f, -1.0f, -0.5f};
    glm_vec3_normalize(light_dir);

    mat4 view, proj;
    camera_get_view_matrix(&app->camera, view);
    glm_perspective(glm_rad(45.0f), (float)app->width / (float)app->height, 0.1f, 100.0f, proj);

    // 1. Shadow Pass
    shadow_calculate_cascades(&g_shadow, view, proj, light_dir, 0.1f, 100.0f);
    for (int i = 0; i < g_shadow.num_cascades; ++i) {
        shadow_begin_pass(&g_shadow, i);
        render_scene(g_shadow.shader_program, true);
    }
    shadow_end_pass(app->width, app->height);

    // 2. SSAO Pass (Usa a primeira cascata de sombra como depth placeholder ou faz pre-pass)
    // Para simplificar no Forward, usamos o depth texture da primeira cascata
    ssao_render(&g_ssao, g_shadow.depth_texture_array, proj, view, app->width, app->height);

    // 3. Main Pass
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(g_pbr_shader);
    glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_projection"), 1, GL_FALSE, (float*)proj);
    glUniform3fv(glGetUniformLocation(g_pbr_shader, "u_view_pos"), 1, (float*)app->camera.position);
    glUniform3fv(glGetUniformLocation(g_pbr_shader, "u_light_dir"), 1, light_dir);
    glUniform3f(glGetUniformLocation(g_pbr_shader, "u_light_color"), 1.0f, 1.0f, 0.9f);
    glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_light_matrices"), 4, GL_FALSE, (float*)g_shadow.light_space_matrices);
    glUniform1fv(glGetUniformLocation(g_pbr_shader, "u_cascade_splits"), 4, g_shadow.cascade_splits);

    if (g_has_ibl) {
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, g_ibl.irradiance_map);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_CUBE_MAP, g_ibl.prefilter_map);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, g_ibl.brdf_lut);
    }
    
    glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, g_ssao.blur_color_buffer);
    shadow_bind_texture(&g_shadow, 8);
    
    render_scene(g_pbr_shader, false);
}

static void OnShutdown() {
    if (g_3d_sound) audio_source_destroy(g_3d_sound);
    if (g_has_ibl) pbr_destroy_ibl(&g_ibl);
    shadow_destroy(&g_shadow);
    ssao_destroy(&g_ssao);
    if (g_pbr_shader) glDeleteProgram(g_pbr_shader);
}

int main(int argc, char* argv[]) {
    SLF_Config config = { 
        .on_start = OnStart, 
        .on_update = OnUpdate, 
        .on_render = OnRender,
        .on_shutdown = OnShutdown 
    };
    return starlight_framework_init("Starlight Next-Gen Showcase", 1280, 720, &config);
}
