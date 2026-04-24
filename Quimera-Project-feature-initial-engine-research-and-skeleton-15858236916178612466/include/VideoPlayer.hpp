// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Video Player (pl_mpeg)
// Licença: MIT | Origem: phoboslab/pl_mpeg
// ============================================================

#include <string>
#include "Log.hpp"

// Forward declaration - implementação em libs_impl.cpp
typedef struct plm_t plm_t;

namespace Vamos {
    class VideoPlayer {
    public:
        bool Open(const std::string& path);
        void Update(double deltaTime);
        void Close();

        int GetWidth() const { return width; }
        int GetHeight() const { return height; }
        double GetDuration() const { return duration; }
        bool IsPlaying() const { return playing; }

        // Ponteiro para pixels RGBA do frame atual
        const uint8_t* GetFrameData() const { return frameData; }

    private:
        plm_t* plm = nullptr;
        int width = 0, height = 0;
        double duration = 0;
        bool playing = false;
        uint8_t* frameData = nullptr;
    };
}
