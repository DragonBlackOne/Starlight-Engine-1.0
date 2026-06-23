#pragma once
#include <memory>
#include <string>

namespace starlight {
    namespace rhi {
        
        enum class BufferType {
            Vertex,
            Index,
            Uniform,
            Storage
        };

        class Pipeline {
        public:
            virtual ~Pipeline() = default;
            virtual void Bind() = 0;
        };

        class Buffer {
        public:
            virtual ~Buffer() = default;
            virtual void Update(const void* data, size_t size) = 0;
            virtual size_t GetSize() const = 0;
        };

        class Texture {
        public:
            virtual ~Texture() = default;
            virtual uint32_t GetWidth() const = 0;
            virtual uint32_t GetHeight() const = 0;
            virtual uint32_t GetNativeHandle() const = 0;
        };

        class CommandBuffer {
        public:
            virtual ~CommandBuffer() = default;
            virtual void Begin() = 0;
            virtual void End() = 0;
            virtual void Draw(uint32_t vertexCount, uint32_t startVertex) = 0;
            virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) = 0;
        };

        class RenderDevice {
        public:
            virtual ~RenderDevice() = default;
            virtual std::shared_ptr<Buffer> CreateBuffer(BufferType type, size_t size, const void* initialData = nullptr) = 0;
            virtual std::shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, uint32_t format, const void* pixelData = nullptr) = 0;
            virtual std::shared_ptr<Pipeline> CreatePipeline() = 0;
            virtual std::shared_ptr<CommandBuffer> GetCommandBuffer() = 0;
            virtual void Submit(std::shared_ptr<CommandBuffer> cmd) = 0;
            
            static std::unique_ptr<RenderDevice> CreateOpenGLDevice();
            static std::unique_ptr<RenderDevice> CreateVulkanDevice();
        };

        class VulkanRenderDevice : public RenderDevice {
        public:
            VulkanRenderDevice();
            ~VulkanRenderDevice() override;
            std::shared_ptr<Buffer> CreateBuffer(BufferType type, size_t size, const void* initialData = nullptr) override;
            std::shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, uint32_t format, const void* pixelData = nullptr) override;
            std::shared_ptr<Pipeline> CreatePipeline() override;
            std::shared_ptr<CommandBuffer> GetCommandBuffer() override;
            void Submit(std::shared_ptr<CommandBuffer> cmd) override;
        };

    }
}
