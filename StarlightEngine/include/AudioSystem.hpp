#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "EngineSystem.hpp"

namespace starlight {
enum class WaveType { Square, Saw, Triangle, Sine, Noise };

struct ChiptuneVoice {
    float frequency = 440.0f;
    float amplitude = 0.5f;
    float phase = 0.0f;
    WaveType type = WaveType::Square;
    bool active = false;
    float timer = 0.0f;
    float duration = 0.0f;
    bool is3D = false;
    float pos[3] = {0, 0, 0};

    // ADSR Envelope
    float attack = 0.01f;
    float decay = 0.1f;
    float sustain = 0.5f;
    float release = 0.2f;
    float envLevel = 0.0f;
    int state = 0;  // 0: Idle, 1: Attack, 2: Decay, 3: Sustain, 4: Release
};

struct FMOperator {
    float phase = 0.0f;
    float frequencyMult = 1.0f;
    float amplitude = 1.0f;

    // ADSR Envelope
    float attack = 0.1f;
    float decay = 0.1f;
    float sustain = 0.7f;
    float release = 0.3f;
    float envLevel = 0.0f;
    int state = 0;  // 0: Idle, 1: Attack, 2: Decay, 3: Sustain, 4: Release
};

struct FMVoice {
    FMOperator ops[4];
    int algorithm = 0;
    float feedback = 0.0f;
    float frequency = 440.0f;
    bool active = false;
    float timer = 0.0f;
    float duration = 0.0f;
    float prevOp1 = 0.0f;  // For feedback
    bool is3D = false;
    float pos[3] = {0, 0, 0};
};

class AudioSystem : public ISystem {
public:
    AudioSystem();
    ~AudioSystem();

    // ISystem
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override {
        return "AudioSystem";
    }
    bool IsMainThreadOnly() const override { return true; }

    void PlayEffect(const std::string& path);
    void Play3DEffect(const std::string& path, float x, float y, float z);
    void SetMasterVolume(float volume);
    void SetListenerPosition(float x, float y, float z, float dx, float dy, float dz);

    // Music Streaming
    void PlayMusic(const std::string& path, bool loop = true, float volume = 1.0f);
    void StopMusic();

    void* GetEngineHandle() {
        return m_audioEngine;
    }

    // Retro APIs
    void PlayNote(float freq, float duration, WaveType type = WaveType::Square);
    void Play3DNote(float freq, float duration, float x, float y, float z, WaveType type = WaveType::Square);
    void PlayFMNote(float freq, float duration, int algorithm = 0);
    void SetOcclusion(float occlusionFactor);

    // Music & Volume APIs
    void SetMusicVolume(float volume);
    float GetMusicVolume() const {
        return m_musicVolume;
    }
    void SetEffectsVolume(float volume);
    float GetEffectsVolume() const {
        return m_effectsVolume;
    }
    bool IsMusicPlaying();
    void PauseMusic();
    void ResumeMusic();
    void SetLowPassCutoff(float cutoff);
    float GetLowPassCutoff() const {
        return m_lowPassCutoff;
    }
    void SetEnvelope(float attack, float decay, float sustain, float release);

    std::mutex m_audioMutex;

    // Process audio callback needs access to these
    const std::vector<ChiptuneVoice>& GetVoices() const {
        return m_voices;
    }
    std::vector<ChiptuneVoice>& GetVoices() {
        return m_voices;
    }
    const std::vector<FMVoice>& GetFMVoices() const {
        return m_fmVoices;
    }
    std::vector<FMVoice>& GetFMVoices() {
        return m_fmVoices;
    }
    float GetLowPassCutoffInternal() const {
        return m_lowPassCutoff;
    }
    float& GetLPLastL() {
        return m_lpLastL;
    }
    float& GetLPLastR() {
        return m_lpLastR;
    }

private:
    std::vector<ChiptuneVoice> m_voices;
    std::vector<FMVoice> m_fmVoices;

    float m_lowPassCutoff = 1.0f;  // 0.0 to 1.0
    float m_lpLastL = 0.0f, m_lpLastR = 0.0f;

    float m_musicVolume = 1.0f;
    float m_effectsVolume = 1.0f;

    // Envelope defaults
    float m_envAttack = 0.01f;
    float m_envDecay = 0.1f;
    float m_envSustain = 0.5f;
    float m_envRelease = 0.2f;

    void* m_audioEngine = nullptr;  // Opaque pointer to ma_engine
    bool m_initialized = false;
};
}  // namespace starlight
