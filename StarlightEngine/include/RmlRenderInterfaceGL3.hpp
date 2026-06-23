#pragma once
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Types.h>

namespace starlight {

    class RmlRenderInterfaceGL3 : public Rml::RenderInterface {
    public:
        RmlRenderInterfaceGL3();
        virtual ~RmlRenderInterfaceGL3();

        // Rml::RenderInterface Implementation
        void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
        
        Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) override;
        void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
        void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;

        bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;
        void ReleaseTexture(Rml::TextureHandle texture_handle) override;

        void EnableScissorRegion(bool enable) override;
        void SetScissorRegion(int x, int y, int width, int height) override;

    private:
        unsigned int m_shader = 0;
        int m_shaderTranslationLocation = -1;
        int m_shaderUseTextureLocation = -1;
        int m_viewportWidth = 1280;
        int m_viewportHeight = 720;
        bool m_scissorEnabled = false;
        int m_scissorX = 0, m_scissorY = 0, m_scissorWidth = 0, m_scissorHeight = 0;

        void SetupShader();
    };

}
