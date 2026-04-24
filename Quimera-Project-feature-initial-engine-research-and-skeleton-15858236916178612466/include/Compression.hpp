// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Compression Module (LZ4 + Zstd)
// LZ4: BSD | Zstd: BSD | Origem: Facebook/lz4
// ============================================================

#include "lz4/lib/lz4.h"
#include <vector>
#include <string>
#include "Log.hpp"

namespace Vamos {
    class Compression {
    public:
        // ---- LZ4: Compressão ultra-rápida ----
        static std::vector<char> CompressLZ4(const void* data, size_t size) {
            int maxDst = LZ4_compressBound((int)size);
            std::vector<char> compressed(maxDst + sizeof(int));

            // Guardar tamanho original no início
            *(int*)compressed.data() = (int)size;

            int compressedSize = LZ4_compress_default(
                (const char*)data, compressed.data() + sizeof(int),
                (int)size, maxDst);

            if (compressedSize <= 0) {
                VAMOS_CORE_ERROR("[Compression] LZ4: Falha na compressão");
                return {};
            }

            compressed.resize(compressedSize + sizeof(int));
            float ratio = (float)compressed.size() / size * 100.0f;
            VAMOS_CORE_TRACE("[Compression] LZ4: {0} -> {1} bytes ({2:.1f}%)", size, compressed.size(), ratio);
            return compressed;
        }

        static std::vector<char> DecompressLZ4(const void* data, size_t compressedSize) {
            int originalSize = *(const int*)data;
            std::vector<char> decompressed(originalSize);

            int result = LZ4_decompress_safe(
                (const char*)data + sizeof(int), decompressed.data(),
                (int)(compressedSize - sizeof(int)), originalSize);

            if (result < 0) {
                VAMOS_CORE_ERROR("[Compression] LZ4: Falha na descompressão");
                return {};
            }

            return decompressed;
        }

        // Conveniência para strings
        static std::vector<char> CompressString(const std::string& str) {
            return CompressLZ4(str.data(), str.size());
        }
    };
}
