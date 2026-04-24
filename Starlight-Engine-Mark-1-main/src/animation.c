// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "animation.h"
#include <math.h>

static int find_key_index(float time, float* times, int count) {
    for (int i = 0; i < count - 1; i++) {
        if (time < times[i + 1]) return i;
    }
    return count - 1;
}

void animation_sample(const Animation* anim, float time, int node_index, mat4 out_matrix) {
    glm_mat4_identity(out_matrix);
    
    for (int i = 0; i < anim->channel_count; i++) {
        if (anim->channels[i].node_index != node_index) continue;
        
        AnimationChannel* chan = &anim->channels[i];
        
        // 1. Translation
        vec3 t = {0, 0, 0};
        if (chan->translation_count > 0) {
            int k = 0; // Busca simplificada
            float t1 = chan->translations[k].time;
            float t2 = chan->translations[k+1].time;
            float factor = (time - t1) / (t2 - t1);
            glm_vec3_lerp(chan->translations[k].value, chan->translations[k+1].value, factor, t);
        }

        // 2. Rotation (SLERP)
        versor q = {0, 0, 0, 1};
        if (chan->rotation_count > 0) {
            int k = 0;
            float t1 = chan->rotations[k].time;
            float t2 = chan->rotations[k+1].time;
            float factor = (time - t1) / (t2 - t1);
            glm_quat_slerp(chan->rotations[k].value, chan->rotations[k+1].value, factor, q);
        }

        // 3. Scale
        vec3 s = {1, 1, 1};
        if (chan->scale_count > 0) {
            int k = 0;
            float t1 = chan->scales[k].time;
            float t2 = chan->scales[k+1].time;
            float factor = (time - t1) / (t2 - t1);
            glm_vec3_lerp(chan->scales[k].value, chan->scales[k+1].value, factor, s);
        }

        // Build Local Matrix
        glm_translate(out_matrix, t);
        mat4 rot_mat; glm_quat_mat4(q, rot_mat);
        glm_mat4_mul(out_matrix, rot_mat, out_matrix);
        glm_scale(out_matrix, s);
        return;
    }
}
