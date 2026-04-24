// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "TitanAudio.hpp"
#include "Log.hpp"
#include <cmath>

#ifdef PlaySound
#undef PlaySound
#endif

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace titan {
    struct InternalAudioState {
        ma_engine engine;
        ma_device device;
        bool deviceActive = false;
    };

    static float UpdateEnvelope(FMOperator& op, float dt) {
        switch (op.state) {
        case 1: // Attack
            op.envLevel += dt / (op.attack + 0.001f);
            if (op.envLevel >= 1.0f) { op.envLevel = 1.0f; op.state = 2; }
            break;
        case 2: // Decay
            op.envLevel -= dt / (op.decay + 0.001f);
            if (op.envLevel <= op.sustain) { op.envLevel = op.sustain; op.state = 3; }
            break;
        case 4: // Release
            op.envLevel -= dt / (op.release + 0.001f);
            if (op.envLevel <= 0.0f) { op.envLevel = 0.0f; op.state = 0; }
            break;
        }
        return op.envLevel;
    }

    static float GetFMSample(FMVoice& voice, float dt) {
        float out = 0.0f;
        float opOut[4];

        // Process Operators and Envelopes
        for (int i = 0; i < 4; i++) {
            voice.ops[i].phase += (voice.frequency * voice.ops[i].frequencyMult) * dt;
            if (voice.ops[i].phase > 1.0f) voice.ops[i].phase -= 1.0f;
            UpdateEnvelope(voice.ops[i], dt);
        }

        auto sinOp = [](float phase, float mod) {
            return std::sin(phase * 6.28318f + mod);
        };

        // Op1: Modulator (w/ Feedback)
        float feedbackSample = sinOp(voice.ops[0].phase, voice.prevOp1 * voice.feedback);
        voice.prevOp1 = feedbackSample;
        opOut[0] = feedbackSample * voice.ops[0].amplitude * voice.ops[0].envLevel;

        // Algorithms based on YM2612
        switch (voice.algorithm) {
        case 0: // 1->2->3->4 (Serial)
            opOut[1] = sinOp(voice.ops[1].phase, opOut[0]) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, opOut[1]) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            out = sinOp(voice.ops[3].phase, opOut[2]) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            break;
        case 1: // (1+2)->3->4
            opOut[1] = sinOp(voice.ops[1].phase, 0) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, opOut[0] + opOut[1]) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            out = sinOp(voice.ops[3].phase, opOut[2]) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            break;
        case 2: // 1->(2+3)->4
            opOut[1] = sinOp(voice.ops[1].phase, 0) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, opOut[0] + opOut[1]) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            out = sinOp(voice.ops[3].phase, opOut[2]) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            break;
        case 3: // (1->2)+3 -> 4
            opOut[1] = sinOp(voice.ops[1].phase, opOut[0]) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, 0) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            out = sinOp(voice.ops[3].phase, opOut[1] + opOut[2]) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            break;
        case 4: // 1->2, 3->4 (Parallel modulators)
            opOut[1] = sinOp(voice.ops[1].phase, opOut[0]) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, 0) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            opOut[3] = sinOp(voice.ops[3].phase, opOut[2]) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            out = (opOut[1] + opOut[3]) * 0.5f;
            break;
        case 5: // 1->(2+3+4)
            opOut[1] = sinOp(voice.ops[1].phase, opOut[0]) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, opOut[0]) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            opOut[3] = sinOp(voice.ops[3].phase, opOut[0]) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            out = (opOut[1] + opOut[2] + opOut[3]) / 3.0f;
            break;
        case 6: // 1->2, 3, 4
            opOut[1] = sinOp(voice.ops[1].phase, opOut[0]) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, 0) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            opOut[3] = sinOp(voice.ops[3].phase, 0) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            out = (opOut[1] + opOut[2] + opOut[3]) / 3.0f;
            break;
        case 7: // 1, 2, 3, 4 (Organ)
            opOut[0] = sinOp(voice.ops[0].phase, 0) * voice.ops[0].amplitude * voice.ops[0].envLevel;
            opOut[1] = sinOp(voice.ops[1].phase, 0) * voice.ops[1].amplitude * voice.ops[1].envLevel;
            opOut[2] = sinOp(voice.ops[2].phase, 0) * voice.ops[2].amplitude * voice.ops[2].envLevel;
            opOut[3] = sinOp(voice.ops[3].phase, 0) * voice.ops[3].amplitude * voice.ops[3].envLevel;
            out = (opOut[0] + opOut[1] + opOut[2] + opOut[3]) * 0.25f;
            break;
        }

        return out;
    }

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        AudioSystem* audioSystem = (AudioSystem*)pDevice->pUserData;
        float* fOutput = (float*)pOutput;
        float sampleRate = (float)pDevice->sampleRate;
        float dt = 1.0f / sampleRate;

        for (ma_uint32 i = 0; i < frameCount; ++i) {
            float mixedSample = 0.0f;
            
            // 1. Chiptune Voices (PSG)
            for (auto& voice : audioSystem->m_voices) {
                if (!voice.active) continue;
                voice.phase += voice.frequency * dt;
                if (voice.phase > 1.0f) voice.phase -= 1.0f;
                
                float vs = 0.0f;
                switch(voice.type) {
                    case WaveType::Square: vs = (voice.phase < 0.5f) ? 1.0f : -1.0f; break;
                    case WaveType::Saw: vs = (voice.phase * 2.0f) - 1.0f; break;
                    case WaveType::Triangle: vs = (voice.phase < 0.5f) ? (voice.phase * 4.0f - 1.0f) : (3.0f - voice.phase * 4.0f); break;
                    case WaveType::Sine: vs = std::sin(voice.phase * 6.28318f); break;
                    case WaveType::Noise: vs = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f; break;
                }
                mixedSample += vs * voice.amplitude;
            }

            // 2. FM Voices (YM2612)
            for (auto& voice : audioSystem->m_fmVoices) {
                if (!voice.active) continue;
                mixedSample += GetFMSample(voice, dt) * 0.5f;
            }

            for (ma_uint32 channel = 0; channel < pDevice->playback.channels; ++channel) {
                fOutput[i * pDevice->playback.channels + channel] = mixedSample;
            }
        }
    }

    AudioSystem::AudioSystem() : m_audioEngine(nullptr), m_initialized(false) {
        m_voices.resize(4);
        m_fmVoices.resize(6); // 6 YM2612 channels
    }

    AudioSystem::~AudioSystem() { Shutdown(); }

    void AudioSystem::Initialize() {
        auto state = new InternalAudioState();
        m_audioEngine = state;

        if (ma_engine_init(NULL, &state->engine) != MA_SUCCESS) return;

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
        Log::Info("AudioSystem: YM2612 FM Core & PSG Synth active.");
    }

    void AudioSystem::Shutdown() {
        if (m_initialized) {
            auto state = (InternalAudioState*)m_audioEngine;
            if (state->deviceActive) ma_device_uninit(&state->device);
            ma_engine_uninit(&state->engine);
            delete state;
            m_audioEngine = nullptr;
            m_initialized = false;
        }
    }

    void AudioSystem::PlayEffect(const std::string& path) {
        if (!m_initialized) return;
        ma_engine_play_sound(&((InternalAudioState*)m_audioEngine)->engine, path.c_str(), NULL);
    }

    void AudioSystem::SetMasterVolume(float volume) {
        if (!m_initialized) return;
        ma_engine_set_volume(&((InternalAudioState*)m_audioEngine)->engine, volume);
    }

    void AudioSystem::PlayNote(float freq, float duration, WaveType type) {
        if (!m_initialized) return;
        for (auto& v : m_voices) {
            if (!v.active) {
                v.frequency = freq; v.duration = duration; v.timer = 0.0f; v.type = type; v.active = true; v.phase = 0.0f;
                return;
            }
        }
    }

    void AudioSystem::PlayFMNote(float freq, float duration, int algorithm) {
        if (!m_initialized) return;
        for (auto& v : m_fmVoices) {
            if (!v.active) {
                v.frequency = freq; v.duration = duration; v.timer = 0.0f; v.algorithm = algorithm; v.active = true;
                // Setup iconic FM Bass patch
                v.feedback = 0.8f;
                v.ops[0].frequencyMult = 0.5f; v.ops[0].amplitude = 0.7f; v.ops[0].state = 1;
                v.ops[1].frequencyMult = 1.0f; v.ops[1].amplitude = 0.8f; v.ops[1].state = 1;
                v.ops[2].frequencyMult = 2.0f; v.ops[2].amplitude = 0.4f; v.ops[2].state = 1;
                v.ops[3].frequencyMult = 1.0f; v.ops[3].amplitude = 1.0f; v.ops[3].state = 1;
                return;
            }
        }
    }

    void AudioSystem::UpdateVoices(float dt) {
        for (auto& v : m_voices) if (v.active) { v.timer += dt; if (v.timer >= v.duration) v.active = false; }
        for (auto& v : m_fmVoices) if (v.active) { v.timer += dt; if (v.timer >= v.duration) v.active = false; }
    }
}
