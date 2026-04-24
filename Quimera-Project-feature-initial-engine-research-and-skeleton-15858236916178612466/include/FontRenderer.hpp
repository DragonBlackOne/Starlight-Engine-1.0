// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Font Rendering Module (stb_truetype + Fontstash)
// Licença: MIT/Public Domain
// ============================================================

#include <GL/glew.h>
#include <string>
#include "Log.hpp"

// Fontstash usa stb_truetype internamente
#define FONTSTASH_IMPLEMENTATION
#define FONS_USE_FREETYPE 0

// Implementação OpenGL do Fontstash
// Este header requer GLEW carregado antes
#ifdef _WIN32
#define GLFONTSTASH_IMPLEMENTATION
#endif

#include "fontstash.h"

namespace Vamos {
    class FontRenderer {
    public:
        FontRenderer(int atlasWidth = 512, int atlasHeight = 512) {
            fs = fonsCreateInternal(&(FONSparams){0});
            if (!fs) {
                VAMOS_CORE_ERROR("FontRenderer: Falha ao criar contexto Fontstash");
                return;
            }
            VAMOS_CORE_INFO("FontRenderer: Fontstash inicializado ({0}x{1})", atlasWidth, atlasHeight);
        }

        ~FontRenderer() {
            if (fs) fonsDeleteInternal(fs);
        }

        int LoadFont(const std::string& name, const std::string& path) {
            int font = fonsAddFont(fs, name.c_str(), path.c_str());
            if (font == FONS_INVALID) {
                VAMOS_CORE_ERROR("FontRenderer: Falha ao carregar fonte {0}", path);
                return -1;
            }
            VAMOS_CORE_INFO("FontRenderer: Fonte '{0}' carregada de {1}", name, path);
            return font;
        }

        void SetFont(int font) {
            fonsSetFont(fs, font);
        }

        void SetSize(float size) {
            fonsSetSize(fs, size);
        }

        void SetColor(unsigned int color) {
            fonsSetColor(fs, color);
        }

        float DrawText(float x, float y, const std::string& text) {
            return fonsDrawText(fs, x, y, text.c_str(), nullptr);
        }

        void GetTextBounds(const std::string& text, float x, float y, float* bounds) {
            fonsTextBounds(fs, x, y, text.c_str(), nullptr, bounds);
        }

        static unsigned int RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
            return (r) | (g << 8) | (b << 16) | (a << 24);
        }

    private:
        FONScontext* fs = nullptr;
    };
}
