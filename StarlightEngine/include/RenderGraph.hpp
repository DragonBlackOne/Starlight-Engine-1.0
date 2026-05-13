// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace starlight {

    /**
     * @brief Blackboard for passing data between passes.
     */
    class RenderGraphBlackboard {
    public:
        template<typename T>
        void Put(const std::string& name, const T& data) {
            m_data[name] = std::make_shared<T>(data);
        }

        template<typename T>
        T* Get(const std::string& name) {
            if (m_data.count(name)) return static_cast<T*>(m_data[name].get());
            return nullptr;
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<void>> m_data;
    };

    /**
     * @brief Description of a transient texture.
     */
    struct RGTextureDesc {
        uint32_t width, height;
        GLenum format = GL_RGBA8;
        GLenum minFilter = GL_LINEAR;
        GLenum magFilter = GL_LINEAR;
    };

    /**
     * @brief Handle to a RenderGraph resource.
     */
    using RGResourceHandle = uint32_t;

    class RenderGraphBuilder {
    public:
        virtual ~RenderGraphBuilder() = default;
        virtual RGResourceHandle CreateTexture(const std::string& name, const RGTextureDesc& desc) = 0;
        virtual void ReadTexture(RGResourceHandle handle) = 0;
        virtual void WriteTexture(RGResourceHandle handle) = 0;
    };

    class RenderGraphResources {
    public:
        virtual ~RenderGraphResources() = default;
        virtual uint32_t GetGLTexture(RGResourceHandle handle) const = 0;
        virtual uint32_t GetGLFBO(RGResourceHandle handle) const = 0;
    };

    /**
     * @brief A pass in the RenderGraph.
     */
    class RenderGraphPass {
    public:
        virtual ~RenderGraphPass() = default;
        virtual void Setup(RenderGraphBuilder& builder) { (void)builder; }
        virtual void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) = 0;
        virtual const char* GetName() const = 0;
    };

    /**
     * @brief Main RenderGraph orchestrator.
     */
    class RenderGraph {
    public:
        RenderGraph();
        ~RenderGraph();

        void Reset();
        
        template<typename T, typename... Args>
        T& AddPass(Args&&... args) {
            auto pass = std::make_unique<T>(std::forward<Args>(args)...);
            T& ref = *pass;
            m_passes.push_back(std::move(pass));
            return ref;
        }

        void Compile();
        void Execute();

        RenderGraphBlackboard& GetBlackboard() { return m_blackboard; }
        
        // Expose a way to register external textures (like the backbuffer or imported targets)
        RGResourceHandle ImportResource(const std::string& name, uint32_t glTextureId, uint32_t glFBO = 0);

    private:
        std::vector<std::unique_ptr<RenderGraphPass>> m_passes;
        RenderGraphBlackboard m_blackboard;
        
        struct PhysicalResource {
            uint32_t glTexture = 0;
            uint32_t glFBO = 0;
            RGTextureDesc desc;
            bool isImported = false;
        };

        std::unordered_map<RGResourceHandle, PhysicalResource> m_physicalResources;
        std::unordered_map<std::string, RGResourceHandle> m_resourceRegistry;
        uint32_t m_nextHandle = 1;

        class BuilderImpl : public RenderGraphBuilder {
        public:
            RenderGraph* graph;
            RGResourceHandle CreateTexture(const std::string& name, const RGTextureDesc& desc) override;
            void ReadTexture(RGResourceHandle handle) override { (void)handle; }
            void WriteTexture(RGResourceHandle handle) override { (void)handle; }
        };

        class ResourcesImpl : public RenderGraphResources {
        public:
            RenderGraph* graph;
            uint32_t GetGLTexture(RGResourceHandle handle) const override;
            uint32_t GetGLFBO(RGResourceHandle handle) const override;
        };

        void Cleanup();
    };

}
