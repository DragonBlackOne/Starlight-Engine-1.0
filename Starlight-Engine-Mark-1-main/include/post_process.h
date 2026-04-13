#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <glad/glad.h>
#include <stdbool.h>

typedef struct {
    GLuint fbo;
    GLuint texture_id;
    GLuint depth_id;
    int width, height;
    int window_width, window_height;

    // Bloom buffers
    GLuint pingpongFBO[2];
    GLuint pingpongColorbuffers[2];

    GLuint post_shader;
    GLuint blur_shader;
    GLuint bright_shader;
    GLuint quadVAO, quadVBO;
} PostProcess;

#include <cglm/cglm.h>

bool post_process_init(PostProcess* pp, int render_w, int render_h, int win_w, int win_h);
void post_process_begin(PostProcess* pp);
void post_process_end(PostProcess* pp);
void post_process_render(PostProcess* pp, float exposure, mat4 view, mat4 proj);
void post_process_destroy(PostProcess* pp);

#endif
