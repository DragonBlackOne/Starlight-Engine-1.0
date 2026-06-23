#pragma once
#include <glad/glad.h>
#include <utility>

namespace starlight {

class GLTexture {
public:
    GLTexture() = default;
    explicit GLTexture(GLuint id) : m_id(id) {}
    ~GLTexture() { Destroy(); }

    GLTexture(const GLTexture&) = delete;
    GLTexture& operator=(const GLTexture&) = delete;

    GLTexture(GLTexture&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}
    GLTexture& operator=(GLTexture&& other) noexcept {
        if (this != &other) {
            Destroy();
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    GLuint Get() const { return m_id; }
    explicit operator bool() const { return m_id != 0; }
    GLuint* Ptr() { return &m_id; }

    void Bind(GLenum target = GL_TEXTURE_2D) const { glBindTexture(target, m_id); }
    void Destroy() {
        if (m_id) { glDeleteTextures(1, &m_id); m_id = 0; }
    }

private:
    GLuint m_id = 0;
};

class GLBuffer {
public:
    GLBuffer() = default;
    explicit GLBuffer(GLuint id) : m_id(id) {}
    ~GLBuffer() { Destroy(); }

    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;

    GLBuffer(GLBuffer&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}
    GLBuffer& operator=(GLBuffer&& other) noexcept {
        if (this != &other) {
            Destroy();
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    GLuint Get() const { return m_id; }
    explicit operator bool() const { return m_id != 0; }
    GLuint* Ptr() { return &m_id; }

    void Bind(GLenum target) const { glBindBuffer(target, m_id); }
    void Destroy() {
        if (m_id) { glDeleteBuffers(1, &m_id); m_id = 0; }
    }

private:
    GLuint m_id = 0;
};

class GLVertexArray {
public:
    GLVertexArray() = default;
    explicit GLVertexArray(GLuint id) : m_id(id) {}
    ~GLVertexArray() { Destroy(); }

    GLVertexArray(const GLVertexArray&) = delete;
    GLVertexArray& operator=(const GLVertexArray&) = delete;

    GLVertexArray(GLVertexArray&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}
    GLVertexArray& operator=(GLVertexArray&& other) noexcept {
        if (this != &other) {
            Destroy();
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    GLuint Get() const { return m_id; }
    explicit operator bool() const { return m_id != 0; }
    GLuint* Ptr() { return &m_id; }

    void Bind() const { glBindVertexArray(m_id); }
    void Destroy() {
        if (m_id) { glDeleteVertexArrays(1, &m_id); m_id = 0; }
    }

private:
    GLuint m_id = 0;
};

class GLFramebuffer {
public:
    GLFramebuffer() = default;
    explicit GLFramebuffer(GLuint id) : m_id(id) {}
    ~GLFramebuffer() { Destroy(); }

    GLFramebuffer(const GLFramebuffer&) = delete;
    GLFramebuffer& operator=(const GLFramebuffer&) = delete;

    GLFramebuffer(GLFramebuffer&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}
    GLFramebuffer& operator=(GLFramebuffer&& other) noexcept {
        if (this != &other) {
            Destroy();
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    GLuint Get() const { return m_id; }
    explicit operator bool() const { return m_id != 0; }
    GLuint* Ptr() { return &m_id; }

    void Bind(GLenum target = GL_FRAMEBUFFER) const { glBindFramebuffer(target, m_id); }
    void Destroy() {
        if (m_id) { glDeleteFramebuffers(1, &m_id); m_id = 0; }
    }

private:
    GLuint m_id = 0;
};

class GLRenderbuffer {
public:
    GLRenderbuffer() = default;
    explicit GLRenderbuffer(GLuint id) : m_id(id) {}
    ~GLRenderbuffer() { Destroy(); }

    GLRenderbuffer(const GLRenderbuffer&) = delete;
    GLRenderbuffer& operator=(const GLRenderbuffer&) = delete;

    GLRenderbuffer(GLRenderbuffer&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}
    GLRenderbuffer& operator=(GLRenderbuffer&& other) noexcept {
        if (this != &other) {
            Destroy();
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    GLuint Get() const { return m_id; }
    explicit operator bool() const { return m_id != 0; }
    GLuint* Ptr() { return &m_id; }

    void Bind() const { glBindRenderbuffer(GL_RENDERBUFFER, m_id); }
    void Destroy() {
        if (m_id) { glDeleteRenderbuffers(1, &m_id); m_id = 0; }
    }

private:
    GLuint m_id = 0;
};

} // namespace starlight
