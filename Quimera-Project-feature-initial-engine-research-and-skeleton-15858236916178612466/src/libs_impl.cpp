// ============================================================
// Vamos Engine - Header-Only Libraries Implementation
// Aqui definimos as implementações das libs header-only
// Este ficheiro é compilado UMA VEZ para gerar os símbolos
// ============================================================

// ---- dr_libs (MIT/Public Domain) ----
// Decodificação de áudio: WAV, MP3, FLAC
#define DR_WAV_IMPLEMENTATION
#include "dr_libs/dr_wav.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_libs/dr_mp3.h"

#define DR_FLAC_IMPLEMENTATION
#include "dr_libs/dr_flac.h"

// ---- FastNoiseLite (MIT) ----
// Geração procedural: Perlin, Simplex, Cellular, etc.
#define FNL_IMPL
#include "FastNoiseLite/Cpp/FastNoiseLite.h"

// ---- meshoptimizer (MIT) ----
// Otimização de meshes, LOD, simplificação
// (compilado via CMake subdirectory)

// ---- tinygltf (MIT) ----
// Carregamento de modelos glTF 2.0
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

// ---- tinyobjloader (MIT) ----
// Carregamento de modelos OBJ
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

// ---- pl_mpeg (MIT) ----
// Playback de vídeo MPEG1
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg/pl_mpeg.h"

// ---- (microui e lz4 foram movidos para libs_impl_c.c para compilar como C puro) ----

// ---- Zstandard (BSD) ----
// (Compilado separadamente - muito grande para single-include)
