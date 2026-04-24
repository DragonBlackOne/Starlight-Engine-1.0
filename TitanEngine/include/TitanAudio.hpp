// Este projeto é feito por IA e só o prompt é feito por um humano.
#include <string>
#include <vector>
#include <memory>

namespace titan {
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
    };

    class AudioSystem {
    public:
        AudioSystem();
        ~AudioSystem();

        void Initialize();
        void Shutdown();
        void PlayEffect(const std::string& path);
        void SetMasterVolume(float volume);

        // Retro APIs
        void PlayNote(float freq, float duration, WaveType type = WaveType::Square);
        void PlayFMNote(float freq, float duration, int algorithm = 0);
        void UpdateVoices(float dt);
        void SetOcclusion(float occlusionFactor);

        std::vector<ChiptuneVoice> m_voices;
        std::vector<FMVoice> m_fmVoices;

    private:
        void* m_audioEngine = nullptr; // Opaque pointer to ma_engine
        bool m_initialized = false;
    };
}
