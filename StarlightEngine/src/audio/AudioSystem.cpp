// Starlight Engine: ULTRA-PREMIUM AUDIO SYSTEM
#include "AudioSystem.hpp"
#include "Log.hpp"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

#ifdef PlaySound
#undef PlaySound
#endif

#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#pragma warning(pop)

namespace starlight {
    struct InternalAudioState {
        ma_engine engine;
        ma_device device;
        bool deviceActive = false;
        glm::vec3 listenerPos = {0,0,0};
        glm::vec3 listenerDir = {0,0,1};
        
        std::vector<ma_sound*> activeEffects;
        ma_sound* musicSound = nullptr;
    };

    template<typename T>
    static float UpdateEnvelope(T& op, float dt) {
        switch (op.state) {
        case 1: op.envLevel += dt / (op.attack + 0.0001f); if (op.envLevel >= 1.0f) { op.envLevel = 1.0f; op.state = 2; } break;
        case 2: op.envLevel -= dt / (op.decay + 0.0001f); if (op.envLevel <= op.sustain) { op.envLevel = op.sustain; op.state = 3; } break;
        case 3: break; // Sustain: keep level until duration ends or manual release
        case 4: op.envLevel -= dt / (op.release + 0.0001f); if (op.envLevel <= 0.0f) { op.envLevel = 0.0f; op.state = 0; } break;
        }
        return op.envLevel;
    }

    static float GetFMSample(FMVoice& voice, float dt) {
        float out = 0.0f;
        float opOut[4];
        for (int i = 0; i < 4; i++) {
            voice.ops[i].phase += (voice.frequency * voice.ops[i].frequencyMult) * dt;
            if (voice.ops[i].phase > 1.0f) voice.ops[i].phase -= 1.0f;
            UpdateEnvelope(voice.ops[i], dt);
        }
        auto sinOp = [](float phase, float mod) { return std::sin(phase * 6.28318f + mod); };
        float feedbackSample = sinOp(voice.ops[0].phase, voice.prevOp1 * voice.feedback);
        voice.prevOp1 = feedbackSample;
        opOut[0] = feedbackSample * voice.ops[0].amplitude * voice.ops[0].envLevel;
        switch (voice.algorithm) {
        case 0: opOut[1] = sinOp(voice.ops[1].phase, opOut[0]); opOut[2] = sinOp(voice.ops[2].phase, opOut[1]); out = sinOp(voice.ops[3].phase, opOut[2]); break;
        case 7: out = (sinOp(voice.ops[0].phase, 0) + sinOp(voice.ops[1].phase, 0) + sinOp(voice.ops[2].phase, 0) + sinOp(voice.ops[3].phase, 0)) * 0.25f; break;
        default: out = sinOp(voice.ops[3].phase, opOut[0]); break;
        }
        return out;
    }

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        (void)pInput;
        AudioSystem* audioSystem = (AudioSystem*)pDevice->pUserData;
        InternalAudioState* state = (InternalAudioState*)audioSystem->GetEngineHandle();
        float* fOutput = (float*)pOutput;
        float dt = 1.0f / (float)pDevice->sampleRate;

        // Note: Using a mutex in the audio callback is generally discouraged for low-latency,
        // but here it ensures absolute stability during the v4.0 industrial transition.
        std::lock_guard<std::mutex> lock(((AudioSystem*)audioSystem)->m_audioMutex);

        for (ma_uint32 i = 0; i < frameCount; ++i) {
            float left = 0.0f, right = 0.0f;
            
            auto processVoice = [&](auto& voice) {
                if (!voice.active) return;
                float vs = 0.0f;
                if constexpr (std::is_same_v<std::decay_t<decltype(voice)>, ChiptuneVoice>) {
                    voice.phase += voice.frequency * dt;
                    if (voice.phase > 1.0f) voice.phase -= 1.0f;
                    switch(voice.type) {
                        case WaveType::Square:   vs = (voice.phase < 0.5f) ? 1.0f : -1.0f; break;
                        case WaveType::Sine:     vs = std::sin(voice.phase * 6.28318f); break;
                        case WaveType::Triangle: vs = (voice.phase < 0.5f) ? (voice.phase * 4.0f - 1.0f) : (3.0f - voice.phase * 4.0f); break;
                        case WaveType::Saw:      vs = (voice.phase * 2.0f) - 1.0f; break;
                        case WaveType::Noise:    vs = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f; break;
                    }
                    UpdateEnvelope(voice, dt);
                    vs *= voice.amplitude * voice.envLevel;
                } else {
                    vs = GetFMSample(voice, dt) * 0.5f;
                }

                if (voice.is3D) {
                    glm::vec3 vPos = {voice.pos[0], voice.pos[1], voice.pos[2]};
                    float dist = glm::distance(vPos, state->listenerPos);
                    float atten = 1.0f / (1.0f + dist * 0.1f);
                    glm::vec3 dir = glm::normalize(vPos - state->listenerPos);
                    float pan = glm::dot(dir, glm::cross(state->listenerDir, glm::vec3(0,1,0)));
                    left += vs * atten * (1.0f - pan);
                    right += vs * atten * (1.0f + pan);
                } else {
                    left += vs; right += vs;
                }
            };

            for (auto& v : audioSystem->m_voices) processVoice(v);
            for (auto& v : audioSystem->m_fmVoices) processVoice(v);

            // Simple Low-Pass Filter
            float alpha = audioSystem->m_lowPassCutoff;
            left = alpha * left + (1.0f - alpha) * audioSystem->m_lpLastL;
            right = alpha * right + (1.0f - alpha) * audioSystem->m_lpLastR;
            audioSystem->m_lpLastL = left;
            audioSystem->m_lpLastR = right;

            fOutput[i * 2 + 0] = left;
            fOutput[i * 2 + 1] = right;
        }
    }

    AudioSystem::AudioSystem() : m_audioEngine(nullptr), m_initialized(false) {
        m_voices.resize(8);
        m_fmVoices.resize(6);
    }

    AudioSystem::~AudioSystem() { OnShutdown(); }

    bool AudioSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        auto state = new InternalAudioState();
        m_audioEngine = state;
        if (ma_engine_init(NULL, &state->engine) != MA_SUCCESS) return false;
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = 2;
        deviceConfig.sampleRate = 44100;
        deviceConfig.dataCallback = data_callback;
        deviceConfig.pUserData = this;
        if (ma_device_init(NULL, &deviceConfig, &state->device) == MA_SUCCESS) {
            ma_device_start(&state->device);
            state->deviceActive = true;
        }
        m_initialized = true;
        Log::Info("AudioSystem: Spatial 3D Engine & YM2612 Core active.");
        return true;
    }

    void AudioSystem::OnShutdown() {
        if (m_initialized) {
            auto state = (InternalAudioState*)m_audioEngine;
            
            std::lock_guard<std::mutex> lock(m_audioMutex);
            if (state->musicSound) {
                ma_sound_uninit(state->musicSound);
                delete state->musicSound;
            }
            for (auto s : state->activeEffects) {
                ma_sound_uninit(s);
                delete s;
            }
            state->activeEffects.clear();

            if (state->deviceActive) ma_device_uninit(&state->device);
            ma_engine_uninit(&state->engine);
            delete state;
            m_audioEngine = nullptr; m_initialized = false;
        }
    }

    void AudioSystem::PlayEffect(const std::string& path) {
        if (!m_initialized) return;
        ma_engine_play_sound(&((InternalAudioState*)m_audioEngine)->engine, path.c_str(), NULL);
    }

    void AudioSystem::Play3DEffect(const std::string& path, float x, float y, float z) {
        if (!m_initialized) return;
        auto state = (InternalAudioState*)m_audioEngine;
        
        ma_sound* sound = new ma_sound();
        if (ma_sound_init_from_file(&state->engine, path.c_str(), 0, NULL, NULL, sound) == MA_SUCCESS) {
            ma_sound_set_position(sound, x, y, z);
            ma_sound_start(sound);
            
            std::lock_guard<std::mutex> lock(m_audioMutex);
            state->activeEffects.push_back(sound);
        } else {
            delete sound;
        }
    }

    void AudioSystem::PlayMusic(const std::string& path, bool loop, float volume) {
        if (!m_initialized) return;
        auto state = (InternalAudioState*)m_audioEngine;

        StopMusic();

        std::lock_guard<std::mutex> lock(m_audioMutex);
        state->musicSound = new ma_sound();
        // MA_SOUND_FLAG_STREAM enables disk streaming (Essential for music!)
        if (ma_sound_init_from_file(&state->engine, path.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, state->musicSound) == MA_SUCCESS) {
            ma_sound_set_looping(state->musicSound, loop);
            ma_sound_set_volume(state->musicSound, volume);
            ma_sound_start(state->musicSound);
        } else {
            delete state->musicSound;
            state->musicSound = nullptr;
        }
    }

    void AudioSystem::StopMusic() {
        if (!m_initialized) return;
        auto state = (InternalAudioState*)m_audioEngine;
        
        std::lock_guard<std::mutex> lock(m_audioMutex);
        if (state->musicSound) {
            ma_sound_stop(state->musicSound);
            ma_sound_uninit(state->musicSound);
            delete state->musicSound;
            state->musicSound = nullptr;
        }
    }

    void AudioSystem::SetMasterVolume(float volume) {
        if (!m_initialized) return;
        auto state = (InternalAudioState*)m_audioEngine;
        ma_engine_set_volume(&state->engine, volume);
    }

    void AudioSystem::SetListenerPosition(float x, float y, float z, float dx, float dy, float dz) {
        if (!m_initialized) return;
        auto state = (InternalAudioState*)m_audioEngine;
        state->listenerPos = {x, y, z};
        state->listenerDir = {dx, dy, dz};
        ma_engine_listener_set_position(&state->engine, 0, x, y, z);
        ma_engine_listener_set_direction(&state->engine, 0, dx, dy, dz);
    }

    void AudioSystem::Play3DNote(float freq, float duration, float x, float y, float z, WaveType type) {
        if (!m_initialized) return;
        std::lock_guard<std::mutex> lock(m_audioMutex);
        for (auto& v : m_voices) if (!v.active) {
            v.frequency = freq; v.duration = duration; v.timer = 0.0f; v.type = type; v.active = true;
            v.is3D = true; v.pos[0] = x; v.pos[1] = y; v.pos[2] = z;
            return;
        }
    }

    void AudioSystem::PlayNote(float freq, float duration, WaveType type) {
        if (!m_initialized) return;
        std::lock_guard<std::mutex> lock(m_audioMutex);
        for (auto& v : m_voices) if (!v.active) {
            v.frequency = freq; v.duration = duration; v.timer = 0.0f; v.type = type; v.active = true;
            v.state = 1; v.envLevel = 0.0f; // Start Attack
            v.is3D = false;
            return;
        }
    }

    void AudioSystem::PlayFMNote(float freq, float duration, int algorithm) {
        if (!m_initialized) return;
        std::lock_guard<std::mutex> lock(m_audioMutex);
        for (auto& v : m_fmVoices) if (!v.active) {
            v.frequency = freq; v.duration = duration; v.timer = 0.0f; v.algorithm = algorithm; v.active = true;
            v.is3D = false;
            v.ops[0].state = 1; v.ops[1].state = 1; v.ops[2].state = 1; v.ops[3].state = 1;
            return;
        }
    }

    void AudioSystem::OnUpdate(float dt) {
        if (!m_initialized) return;
        auto state = (InternalAudioState*)m_audioEngine;

        std::lock_guard<std::mutex> lock(m_audioMutex);
        
        // Prune finished effects
        for (auto it = state->activeEffects.begin(); it != state->activeEffects.end(); ) {
            if (ma_sound_at_end(*it)) {
                ma_sound_uninit(*it);
                delete *it;
                it = state->activeEffects.erase(it);
            } else {
                ++it;
            }
        }

        for (auto& v : m_voices) if (v.active) { 
            v.timer += dt; 
            if (v.timer >= v.duration && v.state != 4) v.state = 4; // Start Release
            if (v.state == 0 && v.timer >= v.duration) v.active = false;
        }
        for (auto& v : m_fmVoices) if (v.active) { 
            v.timer += dt; 
            if (v.timer >= v.duration && v.ops[3].state != 4) {
                for(int i=0; i<4; i++) v.ops[i].state = 4; // Start Release
            }
            if (v.ops[3].state == 0 && v.timer >= v.duration) v.active = false;
        }
    }
}
