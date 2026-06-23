#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include "EngineSystem.hpp"
#include "JobSystem.hpp"
#include "Renderer.hpp"

namespace starlight {

    enum class AssetStatus {
        Unloaded,
        Loading,
        Loaded,
        Failed
    };

    struct AssetMetadata {
        std::string path;
        std::string type;
        uint64_t size = 0;
        uint64_t lastModified = 0;
        
        // Texture settings
        bool flipY = true;
        bool generateMipmaps = true;
        int wrapMode = 0; // 0: Repeat, 1: Clamp
        int filterMode = 1; // 0: Nearest, 1: Linear
    };

    class Asset {
    public:
        virtual ~Asset() = default;
        AssetStatus GetStatus() const { return m_status; }
        const std::string& GetPath() const { return m_path; }
        const AssetMetadata& GetMetadata() const { return m_metadata; }

    protected:
        friend class AssetManager;
        std::string m_path;
        AssetMetadata m_metadata;
        std::atomic<AssetStatus> m_status{AssetStatus::Unloaded};
    };

    class TextureAsset : public Asset {
    public:
        ~TextureAsset() override;
        uint32_t GetTextureID() const { return m_textureID; }
    private:
        friend class AssetManager;
        uint32_t m_textureID = 0;
        int m_width, m_height, m_channels;
        unsigned char* m_rawData = nullptr;
    };

    class MeshAsset : public Asset {
    public:
        std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
    private:
        friend class AssetManager;
        std::shared_ptr<Mesh> m_mesh;
        // Raw data for background loading
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };

    /**
     * @brief AssetManager (Phase 8-9 Evolution)
     * Handles background loading, caching, and meta-file tracking.
     */
    class AssetManager : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "AssetManager"; }
        bool IsMainThreadOnly() const override { return true; }

        std::shared_ptr<TextureAsset> LoadTextureAsync(const std::string& path);
        std::shared_ptr<MeshAsset> LoadMeshAsync(const std::string& path);

        bool IsAssetLoaded(const std::string& path) const;
        float GetLoadingProgress() const;

        AssetMetadata LoadMetadata(const std::string& assetPath) const;

    private:
        std::map<std::string, std::shared_ptr<Asset>> m_assets;
        std::map<std::string, AssetMetadata> m_metaCache;
        std::vector<std::shared_ptr<Asset>> m_pendingGPUUploads;
        mutable std::mutex m_mutex;

        void ProcessPendingUploads();
    };

}
