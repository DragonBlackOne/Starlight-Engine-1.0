// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>
#include <cglm/cglm.h>

// Inicializa o sistema de audio
bool audio_init(void);

// Toca um efeito sonoro 2D (Global/UI)
int audio_play_sfx(const char* filepath, float volume);
// Inicia musica de fundo (Global)
bool audio_play_music(const char* filepath, float volume);

// --- Retro Chiptune Synthesis (Gen 1-4) ---
// type: 0 = SINE, 1 = SQUARE, 2 = TRIANGLE, 3 = SAWTOOTH
void audio_play_chiptune(int type, float frequency, float duration_ms, float volume);

// Controles Globais
void audio_stop_music(void);
void audio_set_master_volume(float volume);
void audio_shutdown(void);

// --- Spatial 3D Audio ---

// Atualiza posicao do ouvinte (Geralmente a Camera)
void audio_set_listener(vec3 position, vec3 forward, vec3 up, vec3 velocity);

typedef struct AudioSource AudioSource;

// Cria uma fonte de som 3D local
AudioSource* audio_create_source(const char* filepath);
void audio_source_set_position(AudioSource* source, vec3 pos);
void audio_source_set_velocity(AudioSource* source, vec3 vel);
void audio_source_set_distance(AudioSource* source, float min_dist, float max_dist);
void audio_source_play(AudioSource* source, bool loop, float volume);
void audio_source_set_pitch(AudioSource* source, float pitch);
void audio_source_set_occlusion(AudioSource* source, float occlusion_factor);
bool audio_source_is_playing(AudioSource* source);
void audio_source_stop(AudioSource* source);
void audio_source_destroy(AudioSource* source);

#endif
