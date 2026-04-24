// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "audio.h"
#include "starlight.h"
#include <stdio.h>

// Definicao de Grupos
typedef enum {
    MIXER_MASTER,
    MIXER_SFX,
    MIXER_MUSIC,
    MIXER_UI,
    MIXER_COUNT
} MixerGroup;

static float g_mixer_volumes[MIXER_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };

void starlight_mixer_set_volume(MixerGroup group, float volume) {
    if (group >= MIXER_COUNT) return;
    g_mixer_volumes[group] = volume;
    
    // Atualizar no motor de audio (Miniaudio)
    debug_log("[AUDIO] Mixer Group %d volume set to %.2f", group, volume);
}

void starlight_audio_ducking_update(float delta_time) {
    // Se houver sons no grupo SFX, abaixamos o grupo MUSIC gradualmente
    static float current_duck = 1.0f;
    float target_duck = 1.0f;
    
    // Logica simplificada: aproximacao suave
    current_duck = (current_duck * 0.9f) + (target_duck * 0.1f);
}
