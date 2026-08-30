// Starlight Engine: ULTRA-PREMIUM AUDIO SYSTEM
#include "AudioSystem.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#include "Renderer.hpp"
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include "Log.hpp"
#include "PathResolver.hpp"

#ifdef PlaySound
#undef PlaySound
#endif

#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#pragma warning(pop)

namespace starlight {
struct InternalAudioState {
    ma_engine engine;
    ma_device device;
    bool deviceActive = false;
    glm::vec3 listenerPos = {0, 0, 0};
    glm::vec3 listenerDir = {0, 0, 1};

    std::vector<ma_sound*> activeEffects;
    ma_sound* musicSound = nullptr;
};

template<typename T>
static float UpdateEnvelope(T& op, float dt) {
    switch (op.state) {
        case 1:
            op.envLevel += dt / (op.attack + 0.0001f);
            if (op.envLevel >= 1.0f) {
                op.envLevel = 1.0f;
                op.state = 2;
            }
            break;
        case 2:
            op.envLevel -= dt / (op.decay + 0.0001f);
            if (op.envLevel <= op.sustain) {
                op.envLevel = op.sustain;
                op.state = 3;
            }
            break;
        case 3:
            break;  // Sustain: keep level until duration ends or manual release
        case 4:
            op.envLevel -= dt / (op.release + 0.0001f);
            if (op.envLevel <= 0.0f) {
                op.envLevel = 0.0f;
                op.state = 0;
            }
            break;
    }
    return op.envLevel;
}

static float GetFMSample(FMVoice& voice, float dt) {
    float out = 0.0f;
    float opOut[4];
    for (int i = 0; i < 4; i++) {
        voice.ops[i].phase += (voice.frequency * voice.ops[i].frequencyMult) * dt;
        if (voice.ops[i].phase > 1.0f)
            voice.ops[i].phase -= 1.0f;
        UpdateEnvelope(voice.ops[i], dt);
    }
    auto sinOp = [](float phase, float mod) { return std::sin(phase * 6.28318f + mod); };
    float feedbackSample = sinOp(voice.ops[0].phase, voice.prevOp1 * voice.feedback);
    voice.prevOp1 = feedbackSample;
    opOut[0] = feedbackSample * voice.ops[0].amplitude * voice.ops[0].envLevel;
    switch (voice.algorithm) {
        case 0:
            opOut[1] = sinOp(voice.ops[1].phase, opOut[0]);
            opOut[2] = sinOp(voice.ops[2].phase, opOut[1]);
            out = sinOp(voice.ops[3].phase, opOut[2]);
            break;
        case 7:
            out = (sinOp(voice.ops[0].phase, 0) + sinOp(voice.ops[1].phase, 0) + sinOp(voice.ops[2].phase, 0) +
                      sinOp(voice.ops[3].phase, 0)) *
                  0.25f;
            break;
        default:
            out = sinOp(voice.ops[3].phase, opOut[0]);
            break;
    }
    return out;
}

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    (void)pInput;
    AudioSystem* audioSystem = (AudioSystem*)pDevice->pUserData;
    InternalAudioState* state = (InternalAudioState*)audioSystem->GetEngineHandle();
    float* fOutput = (float*)pOutput;
    float dt = 1.0f / (float)pDevice->sampleRate;
    ma_uint32 channels = pDevice->playback.channels;

    // Note: Using a mutex in the audio callback is generally discouraged for low-latency,
    // but here it ensures absolute stability during the v4.0 industrial transition.
    std::lock_guard<std::mutex> lock(((AudioSystem*)audioSystem)->m_audioMutex);

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        float left = 0.0f, right = 0.0f;

        auto processVoice = [&](auto& voice) {
            if (!voice.active)
                return;
            float vs = 0.0f;
            if constexpr (std::is_same_v<std::decay_t<decltype(voice)>, ChiptuneVoice>) {
                voice.phase += voice.frequency * dt;
                if (voice.phase > 1.0f)
                    voice.phase -= 1.0f;
                switch (voice.type) {
                    case WaveType::Square:
                        vs = (voice.phase < 0.5f) ? 1.0f : -1.0f;
                        break;
                    case WaveType::Sine:
                        vs = std::sin(voice.phase * 6.28318f);
                        break;
                    case WaveType::Triangle:
                        vs = (voice.phase < 0.5f) ? (voice.phase * 4.0f - 1.0f) : (3.0f - voice.phase * 4.0f);
                        break;
                    case WaveType::Saw:
                        vs = (voice.phase * 2.0f) - 1.0f;
                        break;
                    case WaveType::Noise:
                        vs = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
                        break;
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
                glm::vec3 dir = (dist > 0.0001f) ? glm::normalize(vPos - state->listenerPos) : glm::vec3(0, 0, 1);
                float pan = glm::dot(dir, glm::cross(state->listenerDir, glm::vec3(0, 1, 0)));
                left += vs * atten * (1.0f - pan);
                right += vs * atten * (1.0f + pan);
            } else {
                left += vs;
                right += vs;
            }
        };

        for (auto& v : audioSystem->GetVoices())
            processVoice(v);
        for (auto& v : audioSystem->GetFMVoices())
            processVoice(v);

        // Simple Low-Pass Filter
        float alpha = audioSystem->GetLowPassCutoffInternal();
        left = alpha * left + (1.0f - alpha) * audioSystem->GetLPLastL();
        right = alpha * right + (1.0f - alpha) * audioSystem->GetLPLastR();
        audioSystem->GetLPLastL() = left;
        audioSystem->GetLPLastR() = right;

        // Write safely depending on real channels
        if (channels == 1) {
            fOutput[i] = (left + right) * 0.5f;
        } else if (channels == 2) {
            fOutput[i * 2 + 0] = left;
            fOutput[i * 2 + 1] = right;
        } else {
            fOutput[i * channels + 0] = left;
            fOutput[i * channels + 1] = right;
            for (ma_uint32 c = 2; c < channels; ++c) {
                fOutput[i * channels + c] = 0.0f;
            }
        }
    }
}

AudioSystem::AudioSystem() : m_audioEngine(nullptr), m_initialized(false) {
    m_voices.resize(16);
    m_fmVoices.resize(8);
}

AudioSystem::~AudioSystem() {
    OnShutdown();
}

bool AudioSystem::OnInitialize(const EngineContext& context) {
    (void)context;
    auto state = new InternalAudioState();
    m_audioEngine = state;
    if (ma_engine_init(NULL, &state->engine) != MA_SUCCESS) {
        delete state;
        m_audioEngine = nullptr;
        return false;
    }
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

        if (state->deviceActive) {
            ma_device_uninit(&state->device);
            state->deviceActive = false;
        }

        std::lock_guard<std::mutex> lock(m_audioMutex);
        if (state->musicSound) {
            ma_sound_uninit(state->musicSound);
            delete state->musicSound;
            state->musicSound = nullptr;
        }
        for (auto s : state->activeEffects) {
            ma_sound_uninit(s);
            delete s;
        }
        state->activeEffects.clear();

        ma_engine_uninit(&state->engine);
        delete state;
        m_audioEngine = nullptr;
        m_initialized = false;
    }
}

void AudioSystem::PlayEffect(const std::string& path) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;

    std::lock_guard<std::mutex> lock(m_audioMutex);

    // Sound pooling (max 32 simultaneous effects)
    if (state->activeEffects.size() >= 32) {
        ma_sound* oldest = state->activeEffects.front();
        ma_sound_stop(oldest);
        ma_sound_uninit(oldest);
        delete oldest;
        state->activeEffects.erase(state->activeEffects.begin());
    }

    std::string resolved = PathResolver::Resolve(path);
    ma_sound* sound = new ma_sound();
    if (ma_sound_init_from_file(&state->engine, resolved.c_str(), 0, NULL, NULL, sound) == MA_SUCCESS) {
        ma_sound_set_volume(sound, m_effectsVolume);
        ma_sound_start(sound);
        state->activeEffects.push_back(sound);
    } else {
        delete sound;
    }
}

void AudioSystem::Play3DEffect(const std::string& path, float x, float y, float z) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;

    std::lock_guard<std::mutex> lock(m_audioMutex);

    // Sound pooling (max 32 simultaneous effects)
    if (state->activeEffects.size() >= 32) {
        ma_sound* oldest = state->activeEffects.front();
        ma_sound_stop(oldest);
        ma_sound_uninit(oldest);
        delete oldest;
        state->activeEffects.erase(state->activeEffects.begin());
    }

    std::string resolved = PathResolver::Resolve(path);
    ma_sound* sound = new ma_sound();
    if (ma_sound_init_from_file(&state->engine, resolved.c_str(), 0, NULL, NULL, sound) == MA_SUCCESS) {
        ma_sound_set_spatialization_enabled(sound, true);
        ma_sound_set_pinned_listener_index(sound, 0);
        ma_sound_set_position(sound, x, y, z);
        ma_sound_set_volume(sound, m_effectsVolume);
        ma_sound_start(sound);
        state->activeEffects.push_back(sound);
    } else {
        delete sound;
    }
}

void AudioSystem::PlayMusic(const std::string& path, bool loop, float volume) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;

    std::lock_guard<std::mutex> lock(m_audioMutex);

    if (state->musicSound) {
        ma_sound_stop(state->musicSound);
        ma_sound_uninit(state->musicSound);
        delete state->musicSound;
        state->musicSound = nullptr;
    }

    m_musicTrackVolume = std::clamp(volume, 0.0f, 1.0f);
    std::string resolved = PathResolver::Resolve(path);
    state->musicSound = new ma_sound();
    // MA_SOUND_FLAG_STREAM enables disk streaming (Essential for music!)
    if (ma_sound_init_from_file(&state->engine, resolved.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, state->musicSound) ==
        MA_SUCCESS) {
        ma_sound_set_looping(state->musicSound, loop);
        ma_sound_set_volume(state->musicSound, m_musicTrackVolume * m_musicVolume);
        ma_sound_start(state->musicSound);
    } else {
        delete state->musicSound;
        state->musicSound = nullptr;
    }
}

void AudioSystem::StopMusic() {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;

    std::lock_guard<std::mutex> lock(m_audioMutex);
    if (state->musicSound) {
        ma_sound_stop(state->musicSound);
        ma_sound_uninit(state->musicSound);
        delete state->musicSound;
        state->musicSound = nullptr;
    }
}

void AudioSystem::SetMusicVolume(float volume) {
    m_musicVolume = std::clamp(volume, 0.0f, 1.0f);
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    if (state->musicSound) {
        ma_sound_set_volume(state->musicSound, m_musicTrackVolume * m_musicVolume);
    }
}

void AudioSystem::SetEffectsVolume(float volume) {
    m_effectsVolume = std::clamp(volume, 0.0f, 1.0f);
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    for (auto sound : state->activeEffects) {
        ma_sound_set_volume(sound, m_effectsVolume);
    }
}

bool AudioSystem::IsMusicPlaying() {
    if (!m_initialized)
        return false;
    auto state = (InternalAudioState*)m_audioEngine;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    if (state->musicSound) {
        return ma_sound_is_playing(state->musicSound) == MA_TRUE;
    }
    return false;
}

void AudioSystem::PauseMusic() {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    if (state->musicSound) {
        ma_sound_stop(state->musicSound);
    }
}

void AudioSystem::ResumeMusic() {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    if (state->musicSound) {
        ma_sound_start(state->musicSound);
    }
}

void AudioSystem::SetLowPassCutoff(float cutoff) {
    std::lock_guard<std::mutex> lock(m_audioMutex);
    m_lowPassCutoff = std::clamp(cutoff, 0.0f, 1.0f);
}

void AudioSystem::SetEnvelope(float attack, float decay, float sustain, float release) {
    std::lock_guard<std::mutex> lock(m_audioMutex);
    m_envAttack = attack;
    m_envDecay = decay;
    m_envSustain = sustain;
    m_envRelease = release;
}

void AudioSystem::SetMasterVolume(float volume) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    ma_engine_set_volume(&state->engine, volume);
}

void AudioSystem::SetListenerPosition(float x, float y, float z, float dx, float dy, float dz) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    state->listenerPos = {x, y, z};
    state->listenerDir = {dx, dy, dz};
    ma_engine_listener_set_position(&state->engine, 0, x, y, z);
    ma_engine_listener_set_direction(&state->engine, 0, dx, dy, dz);
}

void AudioSystem::SetListenerVelocity(float vx, float vy, float vz) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;
    ma_engine_listener_set_velocity(&state->engine, 0, vx, vy, vz);
}

void AudioSystem::SetDopplerFactor(float factor) {
    m_dopplerFactor = std::clamp(factor, 0.0f, 5.0f);
}

void AudioSystem::SetDistanceAttenuation(float minDistance, float maxDistance, float rolloff) {
    m_minDistance = std::max(0.1f, minDistance);
    m_maxDistance = std::max(m_minDistance + 0.1f, maxDistance);
    m_rolloff = std::clamp(rolloff, 0.0f, 5.0f);
}

void AudioSystem::SetReverbParameters(float roomSize, float damping, float wetGain) {
    m_reverbRoomSize = std::clamp(roomSize, 0.0f, 1.0f);
    m_reverbDamping = std::clamp(damping, 0.0f, 1.0f);
    m_reverbWetGain = std::clamp(wetGain, 0.0f, 1.0f);
}

void AudioSystem::Play3DNote(float freq, float duration, float x, float y, float z, WaveType type) {
    if (!m_initialized)
        return;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    ChiptuneVoice* targetVoice = nullptr;
    float maxTimer = -1.0f;
    for (auto& v : m_voices) {
        if (!v.active) {
            targetVoice = &v;
            break;
        }
        if (v.timer > maxTimer) {
            maxTimer = v.timer;
            targetVoice = &v;
        }
    }
    if (targetVoice) {
        targetVoice->frequency = freq;
        targetVoice->duration = duration;
        targetVoice->timer = 0.0f;
        targetVoice->phase = 0.0f;
        targetVoice->type = type;
        targetVoice->active = true;
        targetVoice->state = 1;
        targetVoice->envLevel = 0.0f;
        targetVoice->is3D = true;
        targetVoice->pos[0] = x;
        targetVoice->pos[1] = y;
        targetVoice->pos[2] = z;
        targetVoice->attack = m_envAttack;
        targetVoice->decay = m_envDecay;
        targetVoice->sustain = m_envSustain;
        targetVoice->release = m_envRelease;
    }
}

void AudioSystem::PlayNote(float freq, float duration, WaveType type) {
    if (!m_initialized)
        return;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    ChiptuneVoice* targetVoice = nullptr;
    float maxTimer = -1.0f;
    for (auto& v : m_voices) {
        if (!v.active) {
            targetVoice = &v;
            break;
        }
        if (v.timer > maxTimer) {
            maxTimer = v.timer;
            targetVoice = &v;
        }
    }
    if (targetVoice) {
        targetVoice->frequency = freq;
        targetVoice->duration = duration;
        targetVoice->timer = 0.0f;
        targetVoice->phase = 0.0f;
        targetVoice->type = type;
        targetVoice->active = true;
        targetVoice->state = 1;
        targetVoice->envLevel = 0.0f;
        targetVoice->is3D = false;
        targetVoice->attack = m_envAttack;
        targetVoice->decay = m_envDecay;
        targetVoice->sustain = m_envSustain;
        targetVoice->release = m_envRelease;
    }
}

void AudioSystem::PlayFMNote(float freq, float duration, int algorithm) {
    if (!m_initialized)
        return;
    std::lock_guard<std::mutex> lock(m_audioMutex);
    FMVoice* targetVoice = nullptr;
    float maxTimer = -1.0f;
    for (auto& v : m_fmVoices) {
        if (!v.active) {
            targetVoice = &v;
            break;
        }
        if (v.timer > maxTimer) {
            maxTimer = v.timer;
            targetVoice = &v;
        }
    }
    if (targetVoice) {
        targetVoice->frequency = freq;
        targetVoice->duration = duration;
        targetVoice->timer = 0.0f;
        targetVoice->algorithm = algorithm;
        targetVoice->active = true;
        targetVoice->is3D = false;
        targetVoice->ops[0].state = 1;
        targetVoice->ops[1].state = 1;
        targetVoice->ops[2].state = 1;
        targetVoice->ops[3].state = 1;
    }
}

void AudioSystem::PlayImpact(float velocity, AudioMaterial material, float x, float y, float z, bool is3D) {
    if (!m_initialized)
        return;

    float normVel = std::clamp(velocity / 20.0f, 0.1f, 2.5f);
    float duration = std::clamp(0.08f + normVel * 0.05f, 0.05f, 0.35f);

    switch (material) {
        case AudioMaterial::Metal: {
            float baseFreq = 880.0f * (0.8f + normVel * 0.4f);
            if (is3D) {
                Play3DNote(baseFreq, duration * 0.5f, x, y, z, WaveType::Triangle);
                Play3DNote(baseFreq * 1.58f, duration, x, y, z, WaveType::Square);
            } else {
                PlayNote(baseFreq, duration * 0.5f, WaveType::Triangle);
                PlayNote(baseFreq * 1.58f, duration, WaveType::Square);
            }
            break;
        }
        case AudioMaterial::Wood: {
            float baseFreq = 220.0f * (0.7f + normVel * 0.3f);
            if (is3D) {
                Play3DNote(baseFreq, duration * 0.6f, x, y, z, WaveType::Triangle);
            } else {
                PlayNote(baseFreq, duration * 0.6f, WaveType::Triangle);
            }
            break;
        }
        case AudioMaterial::Concrete: {
            float baseFreq = 110.0f * (0.8f + normVel * 0.3f);
            if (is3D) {
                Play3DNote(baseFreq, duration * 0.8f, x, y, z, WaveType::Saw);
                Play3DNote(80.0f, duration * 0.5f, x, y, z, WaveType::Noise);
            } else {
                PlayNote(baseFreq, duration * 0.8f, WaveType::Saw);
                PlayNote(80.0f, duration * 0.5f, WaveType::Noise);
            }
            break;
        }
        case AudioMaterial::Flesh: {
            float baseFreq = 140.0f * (0.8f + normVel * 0.5f);
            if (is3D) {
                Play3DNote(baseFreq, duration * 0.6f, x, y, z, WaveType::Saw);
                Play3DNote(90.0f, duration * 0.4f, x, y, z, WaveType::Noise);
            } else {
                PlayNote(baseFreq, duration * 0.6f, WaveType::Saw);
                PlayNote(90.0f, duration * 0.4f, WaveType::Noise);
            }
            break;
        }
        case AudioMaterial::CyberShield: {
            float baseFreq = 587.33f * (0.9f + normVel * 0.3f);
            PlayFMNote(baseFreq, duration * 0.8f, 0);
            break;
        }
        case AudioMaterial::Glass: {
            float baseFreq = 1760.0f * (0.9f + normVel * 0.2f);
            if (is3D) {
                Play3DNote(baseFreq, duration * 0.4f, x, y, z, WaveType::Sine);
                Play3DNote(baseFreq * 1.414f, duration * 0.3f, x, y, z, WaveType::Triangle);
            } else {
                PlayNote(baseFreq, duration * 0.4f, WaveType::Sine);
                PlayNote(baseFreq * 1.414f, duration * 0.3f, WaveType::Triangle);
            }
            break;
        }
    }
}

void AudioSystem::PlayNoise(float duration, int noiseType, float volume) {
    (void)noiseType;
    (void)volume;
    PlayNote(220.0f, duration, WaveType::Noise);
}

void AudioSystem::PlayExplosion(float intensity, float x, float y, float z, bool is3D) {
    float dur = std::clamp(intensity * 0.8f, 0.2f, 2.5f);
    if (is3D) {
        Play3DNote(55.0f * (1.0f / (intensity + 0.1f)), dur, x, y, z, WaveType::Noise);
        Play3DNote(40.0f, dur * 0.6f, x, y, z, WaveType::Sine);
    } else {
        PlayNote(55.0f * (1.0f / (intensity + 0.1f)), dur, WaveType::Noise);
        PlayNote(40.0f, dur * 0.6f, WaveType::Sine);
    }
}

void AudioSystem::PlayLaser(float duration, float startFreq, float endFreq) {
    (void)endFreq;
    PlayNote(startFreq, duration, WaveType::Saw);
}

void AudioSystem::PlayPowerUp(int melodyType) {
    (void)melodyType;
    PlayNote(523.25f, 0.1f, WaveType::Square); // C5
    PlayNote(659.25f, 0.15f, WaveType::Square); // E5
    PlayNote(783.99f, 0.25f, WaveType::Square); // G5
}

void AudioSystem::PlayEngineRev(float rpmNormalized) {
    float freq = 60.0f + std::clamp(rpmNormalized, 0.0f, 1.0f) * 240.0f;
    PlayNote(freq, 0.08f, WaveType::Saw);
}

void AudioSystem::SetDucking(bool enabled, float duckLevel) {
    m_ducking = enabled;
    m_duckLevel = std::clamp(duckLevel, 0.0f, 1.0f);
    if (m_initialized) {
        auto state = (InternalAudioState*)m_audioEngine;
        if (state && state->musicSound) {
            float effVol = m_ducking ? (m_musicVolume * m_duckLevel) : m_musicVolume;
            ma_sound_set_volume(state->musicSound, effVol * m_musicTrackVolume);
        }
    }
}

void AudioSystem::OnUpdate(float dt) {
    if (!m_initialized)
        return;
    auto state = (InternalAudioState*)m_audioEngine;

    // Auto-sync listener orientation and position to active camera
    if (Engine::IsInitialized() && state) {
        auto& renderer = Engine::Get().GetRenderer();
        glm::vec3 camPos = renderer.GetCameraTransform().position;
        glm::mat4 view = renderer.GetViewMatrix();
        glm::vec3 camDir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
        if (glm::length(camDir) > 0.0001f) {
            camDir = glm::normalize(camDir);
        } else {
            camDir = glm::vec3(0, 0, -1);
        }
        state->listenerPos = camPos;
        state->listenerDir = camDir;
        ma_engine_listener_set_position(&state->engine, 0, camPos.x, camPos.y, camPos.z);
        ma_engine_listener_set_direction(&state->engine, 0, camDir.x, camDir.y, camDir.z);
    }

    std::lock_guard<std::mutex> lock(m_audioMutex);

    // Prune finished effects
    for (auto it = state->activeEffects.begin(); it != state->activeEffects.end();) {
        if (ma_sound_at_end(*it)) {
            ma_sound_uninit(*it);
            delete *it;
            it = state->activeEffects.erase(it);
        } else {
            ++it;
        }
    }

    // Process dynamic Reverb Zones on active scene stack
    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (activeScene) {
        auto& reg = activeScene->GetRegistry();
        auto view = reg.view<ReverbZoneComponent, TransformComponent>();
        float closestDist = 99999.0f;
        float closestReverbFactor = 0.0f;
        bool inZone = false;
        
        glm::vec3 listenerPos = state ? state->listenerPos : glm::vec3(0.0f);
        
        view.each([&](auto entity, auto& reverb, auto& trans) {
            (void)entity;
            if (!reverb.active) return;
            float dist = glm::distance(trans.position, listenerPos);
            if (dist < reverb.maxDistance) {
                inZone = true;
                if (dist < closestDist) {
                    closestDist = dist;
                    if (dist <= reverb.minDistance) {
                        closestReverbFactor = reverb.reverbFactor;
                    } else {
                        float t = (reverb.maxDistance - dist) / (reverb.maxDistance - reverb.minDistance);
                        closestReverbFactor = reverb.reverbFactor * t;
                    }
                }
            }
        });

        if (inZone) {
            // High reverb factor dampens high frequencies (low cutoff)
            float targetCutoff = 1.0f - (closestReverbFactor * 0.7f); // max damping at 0.3 cutoff
            m_lowPassCutoff = glm::mix(m_lowPassCutoff, targetCutoff, std::clamp(dt * 5.0f, 0.0f, 1.0f));
        } else {
            m_lowPassCutoff = glm::mix(m_lowPassCutoff, 1.0f, std::clamp(dt * 5.0f, 0.0f, 1.0f));
        }
    }

    for (auto& v : m_voices)
        if (v.active) {
            v.timer += dt;
            if (v.timer >= v.duration && v.state != 4)
                v.state = 4;  // Start Release
            if (v.state == 0 && v.timer >= v.duration)
                v.active = false;
        }
    for (auto& v : m_fmVoices)
        if (v.active) {
            v.timer += dt;
            if (v.timer >= v.duration && v.ops[3].state != 4) {
                for (int i = 0; i < 4; i++)
                    v.ops[i].state = 4;  // Start Release
            }
            if (v.ops[3].state == 0 && v.timer >= v.duration)
                v.active = false;
        }
}
}  // namespace starlight
