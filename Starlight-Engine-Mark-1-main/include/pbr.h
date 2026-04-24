// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef STARLIGHT_PBR_H
#define STARLIGHT_PBR_H

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdbool.h>

// Struct to hold all the Image-Based Lighting maps for PBR
typedef struct {
    GLuint env_cubemap;       // Full HDR Environment Cubemap
    GLuint irradiance_map;    // Convoluted Diffuse Irradiance
    GLuint prefilter_map;     // Pre-filtered Specular Map (Mips)
    GLuint brdf_lut;          // BRDF Integration Map
} PBR_IBL;

// Generates the entire IBL pipeline from an HDR image file.
// Needs to be called with an active OpenGL context.
bool pbr_init_ibl(const char* hdr_filepath, PBR_IBL* out_ibl);
void pbr_destroy_ibl(PBR_IBL* ibl);

#endif // STARLIGHT_PBR_H
