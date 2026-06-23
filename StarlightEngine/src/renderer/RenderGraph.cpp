#include "RenderGraph.hpp"
#include "Log.hpp"
#include "GLStateSentinel.hpp"

namespace starlight {

    RGResourceHandle RenderGraph::BuilderImpl::CreateTexture(const std::string& name, const RGTextureDesc& desc) {
        if (graph->m_resourceRegistry.count(name)) {
            return graph->m_resourceRegistry[name];
        }
        RGResourceHandle handle = graph->m_nextHandle++;
        graph->m_resourceRegistry[name] = handle;
        
        PhysicalResource res;
        res.desc = desc;
        res.isImported = false;
        graph->m_physicalResources[handle] = res;
        
        return handle;
    }

    uint32_t RenderGraph::ResourcesImpl::GetGLTexture(RGResourceHandle handle) const {
        if (graph->m_physicalResources.count(handle)) {
            return graph->m_physicalResources[handle].glTexture;
        }
        return 0;
    }

    uint32_t RenderGraph::ResourcesImpl::GetGLFBO(RGResourceHandle handle) const {
        if (graph->m_physicalResources.count(handle)) {
            return graph->m_physicalResources[handle].glFBO;
        }
        return 0;
    }

    RenderGraph::RenderGraph() {
        Log::Info("RenderGraph: Advanced Pipeline Initialized (Phase 12 - Ultra Edition).");
    }

    RenderGraph::~RenderGraph() {
        Cleanup();
    }

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, uint32_t glTextureId, uint32_t glFBO) {
        if (m_resourceRegistry.count(name)) return m_resourceRegistry[name];
        RGResourceHandle handle = m_nextHandle++;
        m_resourceRegistry[name] = handle;
        PhysicalResource res;
        res.glTexture = glTextureId;
        res.glFBO = glFBO;
        res.isImported = true;
        m_physicalResources[handle] = res;
        return handle;
    }

    void RenderGraph::Reset() {
        // CORREÇÃO: Não limpar m_passes porque as passes são registradas estaticamente no OnInitialize.
        // Se limparmos aqui, ao chamar RecreateFBO na inicialização perderemos todas as passes de renderização.
        // m_passes.clear();
        for (auto it = m_physicalResources.begin(); it != m_physicalResources.end();) {
            if (!it->second.isImported) {
                if (it->second.glTexture) glDeleteTextures(1, &it->second.glTexture);
                if (it->second.glFBO) glDeleteFramebuffers(1, &it->second.glFBO);
                it = m_physicalResources.erase(it);
            } else {
                ++it;
            }
        }
        // Imported resources remain, but we should rebuild the registry so we don't leak handles
        // Actually it's fine to keep them for now
    }

    void RenderGraph::Compile() {
        BuilderImpl builder;
        builder.graph = this;

        for (auto& pass : m_passes) {
            pass->Setup(builder);
        }

        // Allocate physical resources
        for (auto& pair : m_physicalResources) {
            if (!pair.second.isImported && pair.second.glTexture == 0) {
                glGenTextures(1, &pair.second.glTexture);
                glBindTexture(GL_TEXTURE_2D, pair.second.glTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, pair.second.desc.format, pair.second.desc.width, pair.second.desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pair.second.desc.minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pair.second.desc.magFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                
                // Create FBO for this texture
                glGenFramebuffers(1, &pair.second.glFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, pair.second.glFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pair.second.glTexture, 0);
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    Log::Error("RenderGraph FBO incomplete!");
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
    }

    void RenderGraph::Execute() {
        ResourcesImpl resources;
        resources.graph = this;

        for (auto& pass : m_passes) {
            pass->Execute(m_blackboard, resources);
        }
    }

    void RenderGraph::Cleanup() {
        for (auto& pair : m_physicalResources) {
            if (!pair.second.isImported) {
                if (pair.second.glTexture) glDeleteTextures(1, &pair.second.glTexture);
                if (pair.second.glFBO) glDeleteFramebuffers(1, &pair.second.glFBO);
            }
        }
        m_physicalResources.clear();
        m_resourceRegistry.clear();
    }

}
