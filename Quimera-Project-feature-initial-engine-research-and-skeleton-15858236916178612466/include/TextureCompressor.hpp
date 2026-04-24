// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - GPU Texture Compressor (Basis Universal)
// Licença: Apache 2.0 | Origem: BinomialLLC/basis_universal
// ============================================================

#include <basisu_transcoder.h>
#include <vector>
#include "Log.hpp"

namespace Vamos {
    class TextureCompressor {
    public:
        static void Init() {
            basist::basisu_transcoder_init();
            VAMOS_CORE_INFO("[TextureCompressor] Basis Universal Transcoder inicializado.");
        }

        struct TextureInfo {
            uint32_t width;
            uint32_t height;
            uint32_t levels;
            bool hasAlpha;
        };

        // Decodifica arquivo .basis puro da memória para upload na GPU
        static bool Transcode(const std::vector<uint8_t>& fileData, basist::transcoder_texture_format targetFormat, std::vector<uint8_t>& outData, TextureInfo& outInfo) {
            basist::basisu_transcoder transcoder(&codebook);
            if (!transcoder.validate_header(fileData.data(), fileData.size())) {
                VAMOS_CORE_ERROR("[TextureCompressor] Arquivo basis inválido");
                return false;
            }

            basist::basisu_image_info image_info;
            transcoder.get_image_info(fileData.data(), fileData.size(), image_info, 0);
            
            outInfo.width = image_info.m_width;
            outInfo.height = image_info.m_height;
            outInfo.levels = image_info.m_total_levels;
            outInfo.hasAlpha = image_info.m_alpha_flag;

            if (!transcoder.start_transcoding(fileData.data(), fileData.size())) {
                return false;
            }

            uint32_t bytesPerBlock = basist::basis_get_bytes_per_block_or_pixel(targetFormat);
            outData.resize(image_info.m_total_blocks * bytesPerBlock);

            bool success = transcoder.transcode_image_level(
                fileData.data(), fileData.size(), 0, 0,
                outData.data(), outData.size() / bytesPerBlock, targetFormat
            );

            return success;
        }

    private:
        static basist::etc1_global_selector_codebook codebook; // requer instanciamento
    };

    // basist::etc1_global_selector_codebook TextureCompressor::codebook; // <- declarar isto no cpp
}
