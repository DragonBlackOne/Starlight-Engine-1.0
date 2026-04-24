// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <GL/glew.h>
#include <iostream>

namespace Vamos {
    class Framebuffer {
    public:
        Framebuffer(int width, int height) : width(width), height(height) {
            Invalidate();
        }

        ~Framebuffer() {
            glDeleteFramebuffers(1, &fbo);
            glDeleteTextures(1, &colorAttachment);
            glDeleteRenderbuffers(1, &depthAttachment);
        }

        void Invalidate() {
            if (fbo) {
                glDeleteFramebuffers(1, &fbo);
                glDeleteTextures(1, &colorAttachment);
                glDeleteRenderbuffers(1, &depthAttachment);
            }

            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            // Color Buffer
            glGenTextures(1, &colorAttachment);
            glBindTexture(GL_TEXTURE_2D, colorAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

            // Depth Buffer
            glGenRenderbuffers(1, &depthAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "ERRO: Framebuffer incompleto!" << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Bind() const {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glViewport(0, 0, width, height);
        }

        void Unbind() const {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Resize(int newWidth, int newHeight) {
            if (newWidth <= 0 || newHeight <= 0) return;
            if (width == newWidth && height == newHeight) return;
            width = newWidth;
            height = newHeight;
            Invalidate();
        }

        uint32_t GetColorAttachmentRendererID() const { return colorAttachment; }

    private:
        uint32_t fbo = 0;
        uint32_t colorAttachment = 0;
        uint32_t depthAttachment = 0;
        int width = 0;
        int height = 0;
    };
}
