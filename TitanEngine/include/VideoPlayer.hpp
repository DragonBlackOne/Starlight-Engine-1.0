// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <memory>
#include "Log.hpp"

// Forward declaration - implementaÃ§Ã£o interna via pl_mpeg
typedef struct plm_t plm_t;

namespace titan {
    class VideoPlayer {
    public:
        VideoPlayer();
        ~VideoPlayer();

        bool Open(const std::string& path);
        void Update(double deltaTime);
        void Close();

        bool IsPlaying() const { return m_playing; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        // Retorna a textura OpenGL gerada pelo player
        unsigned int GetTextureID() const { return m_textureID; }

    private:
        plm_t* m_plm = nullptr;
        int m_width = 0, m_height = 0;
        bool m_playing = false;
        unsigned int m_textureID = 0;
        unsigned char* m_frameData = nullptr;

        void CreateTexture();
    };
}
