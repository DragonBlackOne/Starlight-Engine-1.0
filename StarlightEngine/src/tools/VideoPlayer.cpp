// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "VideoPlayer.hpp"
#include <glad/glad.h>
#include <cstring>

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4305)
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg.h"
#pragma warning(pop)

namespace starlight {
    VideoPlayer::VideoPlayer() {}

    VideoPlayer::~VideoPlayer() {
        Close();
    }

    bool VideoPlayer::Open(const std::string& path) {
        m_plm = plm_create_with_filename(path.c_str());
        if (!m_plm) {
            Log::Error("Failed to open video: " + path);
            return false;
        }

        m_width = plm_get_width(m_plm);
        m_height = plm_get_height(m_plm);
        
        // Allocate RGB buffer for YCbCr conversion
        if (m_frameData) delete[] m_frameData;
        m_frameData = new unsigned char[m_width * m_height * 3];
        
        plm_set_loop(m_plm, 1); // Loop by default
        m_playing = true;

        CreateTexture();

        Log::Info("Video opened: " + path + " (" + std::to_string(m_width) + "x" + std::to_string(m_height) + ")");
        return true;
    }

    void VideoPlayer::Update(double deltaTime) {
        (void)deltaTime;
        if (!m_playing || !m_plm) return;

        plm_frame_t* frame = plm_decode_video(m_plm);
        if (frame) {
            // Convert YCbCr to RGB using pl_mpeg's built-in converter
            plm_frame_to_rgb(frame, m_frameData, m_width * 3);
            
            // Upload to GPU
            glBindTexture(GL_TEXTURE_2D, m_textureID);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_frameData);
        }

        if (plm_has_ended(m_plm)) {
            m_playing = false;
        }
    }

    void VideoPlayer::Close() {
        if (m_plm) {
            plm_destroy(m_plm);
            m_plm = nullptr;
        }
        if (m_textureID) {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }
        if (m_frameData) {
            delete[] m_frameData;
            m_frameData = nullptr;
        }
        m_playing = false;
    }

    void VideoPlayer::CreateTexture() {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}
