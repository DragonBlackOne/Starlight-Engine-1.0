// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>


#include "EngineSystem.hpp"

namespace starlight {
    enum class WaveType {
        Square,
        Saw,
        Triangle,
        Sine,
        Noise
    };

    struct ChiptuneVoice {
        float frequency = 440.0f;
        float amplitude = 0.5f;
        float phase = 0.0f;
        WaveType type = WaveType::Square;
        bool active = false;
        float timer = 0.0f;
        float duration = 0.0f;
        bool is3D = false;
        float pos[3] = {0,0,0};
        
        // ADSR Envelope
        float attack = 0.01f;
        float decay = 0.1f;
        float sustain = 0.5f;
        float release = 0.2f;
        float envLevel = 0.0f;
        int state = 0; // 0: Idle, 1: Attack, 2: Decay, 3: Sustain, 4: Release
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
        int state = 0; // 0: Idle, 1: Attack, 2: Decay, 3: Sustain, 4: Release
    };

    struct FMVoice {
        FMOperator ops[4];
        int algorithm = 0;
        float feedback = 0.0f;
        float frequency = 440.0f;
        bool active = false;
        float timer = 0.0f;
        float duration = 0.0f;
        float prevOp1 = 0.0f; // For feedback
        bool is3D = false;
        float pos[3] = {0,0,0};
    };

    class AudioSystem : public ISystem {
    public:
        AudioSystem();
        ~AudioSystem();

        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "AudioSystem"; }

        void PlayEffect(const std::string& path);
        void Play3DEffect(const std::string& path, float x, float y, float z);
        void SetMasterVolume(float volume);
        void SetListenerPosition(float x, float y, float z, float dx, float dy, float dz);
        
        // Music Streaming
        void PlayMusic(const std::string& path, bool loop = true, float volume = 1.0f);
        void StopMusic();
        
        void* GetEngineHandle() { return m_audioEngine; }

        // Retro APIs
        void PlayNote(float freq, float duration, WaveType type = WaveType::Square);
        void Play3DNote(float freq, float duration, float x, float y, float z, WaveType type = WaveType::Square);
        void PlayFMNote(float freq, float duration, int algorithm = 0);
        void SetOcclusion(float occlusionFactor);

        std::vector<ChiptuneVoice> m_voices;
        std::vector<FMVoice> m_fmVoices;

        float m_lowPassCutoff = 1.0f; // 0.0 to 1.0
        float m_lpLastL = 0.0f, m_lpLastR = 0.0f;

        std::mutex m_audioMutex;
        
    private:
        void* m_audioEngine = nullptr; // Opaque pointer to ma_engine
        bool m_initialized = false;
    };
}
