// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include "stb_image.h"

namespace Vamos {
    class Texture {
    public:
        unsigned int ID;
        int width, height, nrChannels;

        Texture(const char* path) {
            glGenTextures(1, &ID);
            glBindTexture(GL_TEXTURE_2D, ID);

            // Parametrização padrão
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_set_flip_vertically_on_load(true);
            unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format = GL_RGB;
                if (nrChannels == 4) format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                std::cerr << "ERRO::TEXTURE::FALHA_AO_CARREGAR: " << path << std::endl;
            }
            stbi_image_free(data);
        }

        void Bind(unsigned int unit = 0) {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, ID);
        }
    };
}
