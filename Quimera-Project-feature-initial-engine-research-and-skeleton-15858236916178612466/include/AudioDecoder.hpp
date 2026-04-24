// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Audio Decoder Module (dr_libs)
// Licença: MIT/Public Domain | Origem: mackron/dr_libs
// Suporta: WAV, MP3, FLAC
// ============================================================

#include "dr_libs/dr_wav.h"
#include "dr_libs/dr_mp3.h"
#include "dr_libs/dr_flac.h"
#include <vector>
#include <string>
#include "Log.hpp"

namespace Vamos {
    struct AudioData {
        std::vector<float> samples;
        unsigned int channels = 0;
        unsigned int sampleRate = 0;
        uint64_t totalFrames = 0;
        bool valid = false;
    };

    class AudioDecoder {
    public:
        static AudioData LoadWAV(const std::string& path) {
            AudioData data;
            unsigned int ch, sr;
            drwav_uint64 total;
            float* samples = drwav_open_file_and_read_pcm_frames_f32(path.c_str(), &ch, &sr, &total, nullptr);
            if (samples) {
                data.channels = ch;
                data.sampleRate = sr;
                data.totalFrames = total;
                data.samples.assign(samples, samples + total * ch);
                data.valid = true;
                drwav_free(samples, nullptr);
                VAMOS_CORE_INFO("[AudioDecoder] WAV carregado: {0} ({1}ch, {2}Hz)", path, ch, sr);
            }
            return data;
        }

        static AudioData LoadMP3(const std::string& path) {
            AudioData data;
            drmp3_config cfg;
            drmp3_uint64 total;
            float* samples = drmp3_open_file_and_read_pcm_frames_f32(path.c_str(), &cfg, &total, nullptr);
            if (samples) {
                data.channels = cfg.channels;
                data.sampleRate = cfg.sampleRate;
                data.totalFrames = total;
                data.samples.assign(samples, samples + total * cfg.channels);
                data.valid = true;
                drmp3_free(samples, nullptr);
                VAMOS_CORE_INFO("[AudioDecoder] MP3 carregado: {0}", path);
            }
            return data;
        }

        static AudioData LoadFLAC(const std::string& path) {
            AudioData data;
            unsigned int ch, sr;
            drflac_uint64 total;
            float* samples = drflac_open_file_and_read_pcm_frames_f32(path.c_str(), &ch, &sr, &total, nullptr);
            if (samples) {
                data.channels = ch;
                data.sampleRate = sr;
                data.totalFrames = total;
                data.samples.assign(samples, samples + total * ch);
                data.valid = true;
                drflac_free(samples, nullptr);
                VAMOS_CORE_INFO("[AudioDecoder] FLAC carregado: {0}", path);
            }
            return data;
        }

        static AudioData Load(const std::string& path) {
            auto ext = path.substr(path.find_last_of('.') + 1);
            if (ext == "wav") return LoadWAV(path);
            if (ext == "mp3") return LoadMP3(path);
            if (ext == "flac") return LoadFLAC(path);
            VAMOS_CORE_ERROR("[AudioDecoder] Formato não suportado: {0}", ext);
            return {};
        }
    };
}
