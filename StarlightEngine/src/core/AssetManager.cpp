#include "AssetManager.hpp"
#include "PathResolver.hpp"
#include "Log.hpp"
#include "VFSSystem.hpp"
#include "AssetLoader.hpp"
#include "stb_image.h"
#include "glad/glad.h"
#include <filesystem>
#include <fstream>
#include "json.hpp"
#include <cstring>

using json = nlohmann::json;

namespace starlight {

    bool AssetManager::OnInitialize(const EngineContext& context) {
        (void)context;
        Log::Info("AssetManager: Background Pipeline Initialized (Phase 12).");
        return true;
    }

    TextureAsset::~TextureAsset() {
        if (m_textureID != 0) {
            Renderer::SubmitDeferredTextureDeletion(m_textureID);
        }
    }

    void AssetManager::OnShutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_assets.clear();
    }

    void AssetManager::OnUpdate(float dt) {
        (void)dt;
        ProcessPendingUploads();
    }

    std::shared_ptr<TextureAsset> AssetManager::LoadTextureAsync(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_assets.count(path)) {
            return std::static_pointer_cast<TextureAsset>(m_assets[path]);
        }

        auto asset = std::make_shared<TextureAsset>();
        asset->m_path = path;
        asset->m_status = AssetStatus::Loading;
        asset->m_metadata = LoadMetadata(path); // Load metadata on main thread (fast)
        m_assets[path] = asset;

        // Background Loading Task
        JobSystem::Execute([this, asset, path](uint32_t id) {
            (void)id;
            int w, h, c;
            
            std::string resolved = PathResolver::Resolve(path);
            unsigned char* data = stbi_load(resolved.c_str(), &w, &h, &c, 0);
            
            if (data) {
                if (asset->m_metadata.flipY) {
                    int rowBytes = w * c;
                    std::vector<unsigned char> tempRow(rowBytes);
                    for (int y = 0; y < h / 2; ++y) {
                        unsigned char* row1 = data + y * rowBytes;
                        unsigned char* row2 = data + (h - 1 - y) * rowBytes;
                        std::memcpy(tempRow.data(), row1, rowBytes);
                        std::memcpy(row1, row2, rowBytes);
                        std::memcpy(row2, tempRow.data(), rowBytes);
                    }
                }
                asset->m_rawData = data;
                asset->m_width = w;
                asset->m_height = h;
                asset->m_channels = c;
                
                std::lock_guard<std::mutex> lock2(m_mutex);
                m_pendingGPUUploads.push_back(asset);
            } else {
                asset->m_status = AssetStatus::Failed;
                Log::Error("AssetManager: Failed to load texture: " + path);
            }
        });

        return asset;
    }

    std::shared_ptr<MeshAsset> AssetManager::LoadMeshAsync(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_assets.count(path)) {
            return std::static_pointer_cast<MeshAsset>(m_assets[path]);
        }

        auto asset = std::make_shared<MeshAsset>();
        asset->m_path = path;
        asset->m_status = AssetStatus::Loading;
        asset->m_metadata = LoadMetadata(path);
        m_assets[path] = asset;

        JobSystem::Execute([this, asset, path](uint32_t id) {
            (void)id;
            auto meshData = AssetLoader::LoadGLTF(path);
            
            if (meshData.valid) {
                asset->m_vertices = std::move(meshData.vertices);
                asset->m_indices = std::move(meshData.indices);
                
                std::lock_guard<std::mutex> lock2(m_mutex);
                m_pendingGPUUploads.push_back(asset);
            } else {
                asset->m_status = AssetStatus::Failed;
            }
        });

        return asset;
    }

    void AssetManager::ProcessPendingUploads() {
        std::vector<std::shared_ptr<Asset>> toUpload;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            toUpload = std::move(m_pendingGPUUploads);
            m_pendingGPUUploads.clear();
        }

        for (auto& asset : toUpload) {
            if (auto tex = std::dynamic_pointer_cast<TextureAsset>(asset)) {
                // GPU Upload (Main Thread)
                uint32_t texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);

                GLenum format = (tex->m_channels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, format, tex->m_width, tex->m_height, 0, format, GL_UNSIGNED_BYTE, tex->m_rawData);
                
                if (tex->m_metadata.generateMipmaps) {
                    glGenerateMipmap(GL_TEXTURE_2D);
                }

                GLint wrap = (tex->m_metadata.wrapMode == 0) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

                GLint minFilter = tex->m_metadata.generateMipmaps ? 
                    (tex->m_metadata.filterMode == 0 ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) :
                    (tex->m_metadata.filterMode == 0 ? GL_NEAREST : GL_LINEAR);
                GLint magFilter = (tex->m_metadata.filterMode == 0) ? GL_NEAREST : GL_LINEAR;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

                stbi_image_free(tex->m_rawData);
                tex->m_rawData = nullptr;
                tex->m_textureID = texture;
                tex->m_status = AssetStatus::Loaded;
                
                Log::Info("AssetManager: Texture uploaded to GPU (with Metadata): " + tex->m_path);
            } else if (auto meshAsset = std::dynamic_pointer_cast<MeshAsset>(asset)) {
                // GPU Upload (Main Thread)
                meshAsset->m_mesh = std::make_shared<Mesh>(meshAsset->m_vertices, meshAsset->m_indices);
                
                // Clear RAM data to save memory
                meshAsset->m_vertices.clear();
                meshAsset->m_indices.clear();
                meshAsset->m_vertices.shrink_to_fit();
                meshAsset->m_indices.shrink_to_fit();

                meshAsset->m_status = AssetStatus::Loaded;
                Log::Info("AssetManager: Mesh uploaded to GPU: " + meshAsset->m_path);
            }
        }
    }

    AssetMetadata AssetManager::LoadMetadata(const std::string& assetPath) const {
        std::string metaPath = assetPath + ".meta";
        AssetMetadata meta;
        meta.path = assetPath;

        auto bytes = VFSSystem::Get().ReadFile(metaPath);
        if (!bytes.empty()) {
            try {
                json j = json::parse(bytes.begin(), bytes.end());

                if (j.contains("flipY")) meta.flipY = j["flipY"];
                if (j.contains("generateMipmaps")) meta.generateMipmaps = j["generateMipmaps"];
                if (j.contains("wrapMode")) meta.wrapMode = j["wrapMode"];
                if (j.contains("filterMode")) meta.filterMode = j["filterMode"];
                
                Log::Info("AssetManager: Loaded metadata for " + assetPath);
            } catch (const std::exception& e) {
                Log::Warn("AssetManager: Failed to parse meta file {}: {}", metaPath, e.what());
            }
        }

        return meta;
    }

    bool AssetManager::IsAssetLoaded(const std::string& path) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_assets.count(path)) {
            return m_assets.at(path)->GetStatus() == AssetStatus::Loaded;
        }
        return false;
    }

    float AssetManager::GetLoadingProgress() const {
        // Implementation for overall progress
        return 0.0f; 
    }

    void AssetManager::FlushCache() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto it = m_assets.begin(); it != m_assets.end(); ) {
            if (it->second.use_count() == 1) {
                Log::Info("AssetManager: Flushing unused asset from cache: " + it->first);
                it = m_assets.erase(it);
            } else {
                ++it;
            }
        }
        m_metaCache.clear();
    }

}
