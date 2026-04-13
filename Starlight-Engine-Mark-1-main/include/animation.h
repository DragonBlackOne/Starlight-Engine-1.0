#ifndef ANIMATION_H
#define ANIMATION_H

#include <cglm/cglm.h>
#include <stdbool.h>

typedef struct {
    float time;
    vec3  value;
} TranslationKey;

typedef struct {
    float time;
    versor value; // Quaternion
} RotationKey;

typedef struct {
    float time;
    vec3  value;
} ScaleKey;

typedef struct {
    int node_index;
    TranslationKey* translations;
    int translation_count;
    RotationKey*    rotations;
    int rotation_count;
    ScaleKey*       scales;
    int scale_count;
} AnimationChannel;

typedef struct {
    char name[64];
    float duration;
    AnimationChannel* channels;
    int channel_count;
} Animation;

/**
 * @brief Calcula a matriz de transformacao de um nodo para um tempo especifico.
 */
void animation_sample(const Animation* anim, float time, int node_index, mat4 out_matrix);

#endif
