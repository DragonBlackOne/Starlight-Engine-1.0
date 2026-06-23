#include "RenderDevice.hpp"
#include <glad/glad.h>
#include "Log.hpp"

namespace starlight {
    namespace rhi {

        class GLBuffer : public Buffer {
        public:
            GLBuffer(BufferType type, size_t size, const void* initialData) : m_size(size) {
                switch (type) {
                    case BufferType::Vertex:  m_target = GL_ARRAY_BUFFER; break;
                    case BufferType::Index:   m_target = GL_ELEMENT_ARRAY_BUFFER; break;
                    case BufferType::Uniform: m_target = GL_UNIFORM_BUFFER; break;
                    case BufferType::Storage: m_target = GL_SHADER_STORAGE_BUFFER; break;
                }
                glGenBuffers(1, &m_id);
                glBindBuffer(m_target, m_id);
                glBufferData(m_target, size, initialData, GL_STATIC_DRAW);
                glBindBuffer(m_target, 0);
            }

            ~GLBuffer() override {
                glDeleteBuffers(1, &m_id);
            }

            void Update(const void* data, size_t size) override {
                glBindBuffer(m_target, m_id);
                glBufferSubData(m_target, 0, size, data);
                glBindBuffer(m_target, 0);
                m_size = size;
            }

            size_t GetSize() const override { return m_size; }
            uint32_t GetID() const { return m_id; }

        private:
            uint32_t m_id = 0;
            uint32_t m_target = 0;
            size_t m_size = 0;
        };

        class GLTexture : public Texture {
        public:
            GLTexture(uint32_t width, uint32_t height, uint32_t format, const void* pixelData)
                : m_width(width), m_height(height) {
                (void)format;
                glGenTextures(1, &m_id);
                glBindTexture(GL_TEXTURE_2D, m_id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            ~GLTexture() override {
                glDeleteTextures(1, &m_id);
            }

            uint32_t GetWidth() const override { return m_width; }
            uint32_t GetHeight() const override { return m_height; }
            uint32_t GetNativeHandle() const override { return m_id; }

        private:
            uint32_t m_id = 0;
            uint32_t m_width = 0;
            uint32_t m_height = 0;
        };

        class GLCommandBuffer : public CommandBuffer {
        public:
            GLCommandBuffer() = default;
            void Begin() override {}
            void End() override {}
            void Draw(uint32_t vertexCount, uint32_t startVertex) override {
                glDrawArrays(GL_TRIANGLES, startVertex, vertexCount);
            }
            void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override {
                (void)baseVertex;
                glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(uintptr_t)(startIndex * sizeof(uint32_t)));
            }
        };

        class GLPipeline : public Pipeline {
        public:
            void Bind() override {
                // Bind GL pipeline state
            }
        };

        class GLRenderDevice : public RenderDevice {
        public:
            GLRenderDevice() {
                Log::Info("OpenGL RHI RenderDevice initialized.");
            }

            std::shared_ptr<Buffer> CreateBuffer(BufferType type, size_t size, const void* initialData) override {
                return std::make_shared<GLBuffer>(type, size, initialData);
            }

            std::shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, uint32_t format, const void* pixelData) override {
                return std::make_shared<GLTexture>(width, height, format, pixelData);
            }

            std::shared_ptr<Pipeline> CreatePipeline() override {
                return std::make_shared<GLPipeline>();
            }

            std::shared_ptr<CommandBuffer> GetCommandBuffer() override {
                return std::make_shared<GLCommandBuffer>();
            }

            void Submit(std::shared_ptr<CommandBuffer> cmd) override {
                (void)cmd;
            }
        };

        std::unique_ptr<RenderDevice> RenderDevice::CreateOpenGLDevice() {
            return std::make_unique<GLRenderDevice>();
        }

        // --- Vulkan 1.3 Backend Groundwork ---

        class VKPipeline : public Pipeline {
        public:
            void Bind() override {}
        };

        class VKBuffer : public Buffer {
        public:
            VKBuffer(size_t size) : m_size(size) {}
            void Update(const void* data, size_t size) override { (void)data; m_size = size; }
            size_t GetSize() const override { return m_size; }
        private:
            size_t m_size;
        };

        class VKTexture : public Texture {
        public:
            VKTexture(uint32_t w, uint32_t h) : m_w(w), m_h(h) {}
            uint32_t GetWidth() const override { return m_w; }
            uint32_t GetHeight() const override { return m_h; }
            uint32_t GetNativeHandle() const override { return 0; }
        private:
            uint32_t m_w, m_h;
        };

        class VKCommandBuffer : public CommandBuffer {
        public:
            void Begin() override {}
            void End() override {}
            void Draw(uint32_t vertexCount, uint32_t startVertex) override { (void)vertexCount; (void)startVertex; }
            void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override { (void)indexCount; (void)startIndex; (void)baseVertex; }
        };

        VulkanRenderDevice::VulkanRenderDevice() {
            Log::Info("Vulkan RHI RenderDevice groundwork initialized.");
        }

        VulkanRenderDevice::~VulkanRenderDevice() = default;

        std::shared_ptr<Buffer> VulkanRenderDevice::CreateBuffer(BufferType type, size_t size, const void* initialData) {
            (void)type; (void)initialData;
            return std::make_shared<VKBuffer>(size);
        }

        std::shared_ptr<Texture> VulkanRenderDevice::CreateTexture2D(uint32_t width, uint32_t height, uint32_t format, const void* pixelData) {
            (void)format; (void)pixelData;
            return std::make_shared<VKTexture>(width, height);
        }

        std::shared_ptr<Pipeline> VulkanRenderDevice::CreatePipeline() {
            return std::make_shared<VKPipeline>();
        }

        std::shared_ptr<CommandBuffer> VulkanRenderDevice::GetCommandBuffer() {
            return std::make_shared<VKCommandBuffer>();
        }

        void VulkanRenderDevice::Submit(std::shared_ptr<CommandBuffer> cmd) {
            (void)cmd;
        }

        std::unique_ptr<RenderDevice> RenderDevice::CreateVulkanDevice() {
            return std::make_unique<VulkanRenderDevice>();
        }

    }
}
