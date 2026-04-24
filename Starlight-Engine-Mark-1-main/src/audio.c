// Este projeto é feito por IA e só o prompt é feito por um humano.
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "audio.h"
#include <stdio.h>

static ma_engine g_audio_engine;
static ma_sound  g_music_sound;
static bool      g_music_active = false;
static bool      g_initialized = false;

static ma_sound_group g_sfx_group;
static ma_sound_group g_music_group;

// --- Retro Waveform Synth Pool ---
#define MAX_CHIPTUNES 8
static ma_waveform g_chip_wave[MAX_CHIPTUNES];
static ma_sound    g_chip_sound[MAX_CHIPTUNES];
static int         g_chip_idx = 0;

void audio_apply_ducking(float duck_factor, float time_ms) {
    // Abaixa o volume do grupo de musica baseado na atividade do grupo de SFX
    ma_sound_group_set_volume(&g_music_group, duck_factor);
    // (Em uma versao final, usaríamos um envelope de volume suave)
}

bool audio_init(void) {
    if (g_initialized) return true;
    ma_result result = ma_engine_init(NULL, &g_audio_engine);
    if (result != MA_SUCCESS) return false;
    
    ma_sound_group_init(&g_audio_engine, 0, NULL, &g_sfx_group);
    ma_sound_group_init(&g_audio_engine, 0, NULL, &g_music_group);
    
    for (int i = 0; i < MAX_CHIPTUNES; i++) {
        memset(&g_chip_sound[i], 0, sizeof(ma_sound));
        memset(&g_chip_wave[i], 0, sizeof(ma_waveform));
    }
    
    g_initialized = true;
    return true;
}

int audio_play_sfx(const char* filepath, float volume) {
    ma_result result;
    if (!g_initialized) return -1;
    
    result = ma_engine_play_sound(&g_audio_engine, filepath, NULL);
    if (result != MA_SUCCESS) {
        printf("[AUDIO] Erro ao tocar SFX: %s (code %d)\n", filepath, result);
        return -1;
    }
    return 0;
}

// === RETRO SYNTHESIS (8-BIT WAVES) ===
void audio_play_chiptune(int type, float frequency, float duration_ms, float volume) {
    if (!g_initialized) return;
    
    int i = g_chip_idx;
    g_chip_idx = (g_chip_idx + 1) % MAX_CHIPTUNES;
    
    // Uninit anterior slots to prevent overlap memory leaks
    ma_sound_uninit(&g_chip_sound[i]);
    ma_waveform_uninit(&g_chip_wave[i]);
    
    ma_waveform_config config = ma_waveform_config_init(
        ma_format_f32,
        2, 
        ma_engine_get_sample_rate(&g_audio_engine), 
        (ma_waveform_type)type,
        0.2f, // Base Amplitude
        frequency
    );
    ma_waveform_init(&config, &g_chip_wave[i]);
    
    ma_sound_init_from_data_source(&g_audio_engine, &g_chip_wave[i], 0, &g_sfx_group, &g_chip_sound[i]);
    ma_sound_set_volume(&g_chip_sound[i], volume);
    
    // --- ADSR ENVELOPES (Decay/Release Curve) ---
    // Faz a onda decair suavemente do volume máximo até 0 durante seu tempo de vida (Simula ressonância física)
    ma_sound_set_fade_in_milliseconds(&g_chip_sound[i], volume, 0.0f, duration_ms);
    
    // Auto-Destruct envelope literal
    ma_uint64 stop_frame = ma_engine_get_time_in_pcm_frames(&g_audio_engine) + 
                           (ma_uint64)((duration_ms / 1000.0) * ma_engine_get_sample_rate(&g_audio_engine));
    ma_sound_set_stop_time_in_pcm_frames(&g_chip_sound[i], stop_frame);
    
    ma_sound_start(&g_chip_sound[i]);
}

bool audio_play_music(const char* filepath, float volume) {
    if (!g_initialized) return false;
    
    // Parar musica anterior
    if (g_music_active) {
        ma_sound_uninit(&g_music_sound);
        g_music_active = false;
    }
    
    ma_result result = ma_sound_init_from_file(&g_audio_engine, filepath, 0, &g_music_group, NULL, &g_music_sound);
    if (result != MA_SUCCESS) {
        printf("[AUDIO] Erro ao carregar musica: %s (code %d)\n", filepath, result);
        return false;
    }
    
    ma_sound_set_looping(&g_music_sound, MA_TRUE);
    ma_sound_set_volume(&g_music_sound, volume);
    ma_sound_start(&g_music_sound);
    g_music_active = true;
    
    printf("[AUDIO] Musica iniciada: %s\n", filepath);
    return true;
}

void audio_stop_music(void) {
    if (g_music_active) {
        ma_sound_stop(&g_music_sound);
        ma_sound_uninit(&g_music_sound);
        g_music_active = false;
    }
}

void audio_set_master_volume(float volume) {
    if (g_initialized) {
        ma_engine_set_volume(&g_audio_engine, volume);
    }
}

void audio_shutdown(void) {
    if (g_music_active) {
        ma_sound_stop(&g_music_sound);
        ma_sound_uninit(&g_music_sound);
        g_music_active = false;
    }
    if (g_initialized) {
        for (int i = 0; i < MAX_CHIPTUNES; i++) {
            ma_sound_uninit(&g_chip_sound[i]);
            ma_waveform_uninit(&g_chip_wave[i]);
        }
        ma_engine_uninit(&g_audio_engine);
        g_initialized = false;
        printf("[AUDIO] Shutdown completo.\n");
    }
}

// --- Spatial 3D Audio ---

struct AudioSource {
    ma_sound sound;
    bool initialized;
};

void audio_set_listener(vec3 position, vec3 forward, vec3 up, vec3 velocity) {
    if (!g_initialized) return;
    ma_engine_listener_set_position(&g_audio_engine, 0, position[0], position[1], position[2]);
    ma_engine_listener_set_direction(&g_audio_engine, 0, forward[0], forward[1], forward[2]);
    ma_engine_listener_set_world_up(&g_audio_engine, 0, up[0], up[1], up[2]);
    if (velocity) ma_engine_listener_set_velocity(&g_audio_engine, 0, velocity[0], velocity[1], velocity[2]);
}

AudioSource* audio_create_source(const char* filepath) {
    if (!g_initialized) return NULL;
    AudioSource* src = (AudioSource*)malloc(sizeof(AudioSource));
    if (ma_sound_init_from_file(&g_audio_engine, filepath, MA_SOUND_FLAG_DECODE, &g_sfx_group, NULL, &src->sound) != MA_SUCCESS) {
        free(src);
        return NULL;
    }
    src->initialized = true;
    ma_sound_set_spatialization_enabled(&src->sound, MA_TRUE);
    return src;
}

void audio_source_set_position(AudioSource* source, vec3 pos) {
    if (source && source->initialized && pos) {
        ma_sound_set_position(&source->sound, pos[0], pos[1], pos[2]);
    }
}

void audio_source_set_velocity(AudioSource* source, vec3 vel) {
    if (source && source->initialized && vel) {
        ma_sound_set_velocity(&source->sound, vel[0], vel[1], vel[2]);
    }
}

void audio_source_set_distance(AudioSource* source, float min_dist, float max_dist) {
    if (source && source->initialized) {
        ma_sound_set_min_distance(&source->sound, min_dist);
        ma_sound_set_max_distance(&source->sound, max_dist);
    }
}

void audio_source_play(AudioSource* source, bool loop, float volume) {
    if (source && source->initialized) {
        ma_sound_set_looping(&source->sound, loop ? MA_TRUE : MA_FALSE);
        ma_sound_set_volume(&source->sound, volume);
        ma_sound_start(&source->sound);
    }
}

void audio_source_set_pitch(AudioSource* source, float pitch) {
    if (source && source->initialized) {
        ma_sound_set_pitch(&source->sound, pitch);
    }
}

void audio_source_set_occlusion(AudioSource* source, float occlusion_factor) {
    if (source && source->initialized) {
        // occlusion_factor = 0 (Visão direta), 1.0 (Parede grossa/Sólida)
        float clamped = occlusion_factor;
        if (clamped < 0.0f) clamped = 0.0f;
        if (clamped > 1.0f) clamped = 1.0f;

        // Oclusão física afeta severamente o volume, e altera sutilmente o pitch (Frequency LPF Fake)
        float muffling_vol = 1.0f - (clamped * 0.85f); 
        float muffling_pitch = 1.0f - (clamped * 0.25f);

        ma_sound_set_volume(&source->sound, muffling_vol);
        ma_sound_set_pitch(&source->sound, muffling_pitch);
    }
}

bool audio_source_is_playing(AudioSource* source) {
    if (!source || !source->initialized) return false;
    return ma_sound_is_playing(&source->sound);
}

void audio_source_stop(AudioSource* source) {
    if (source && source->initialized) {
        ma_sound_stop(&source->sound);
    }
}

void audio_source_destroy(AudioSource* source) {
    if (source) {
        if (source->initialized) ma_sound_uninit(&source->sound);
        free(source);
    }
}
