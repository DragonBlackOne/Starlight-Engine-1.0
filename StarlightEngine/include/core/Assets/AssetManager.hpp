#pragma once
#include "Asset.hpp"
#include "FileWatcher.hpp"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <future>
#include <random>

namespace starlight::assets {

class AssetManager {
public:
    static AssetManager& Get() {
        static AssetManager instance;
        return instance;
    }

    AssetHandle RegisterAsset(const std::string& virtualPath, AssetType type, const std::string& physicalPath = "") {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_pathToHandle.find(virtualPath);
        if (it != m_pathToHandle.end()) {
            return it->second;
        }

        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis(1, UINT64_MAX);

        AssetHandle handle = dis(gen);
        AssetMetadata meta;
        meta.handle = handle;
        meta.type = type;
        meta.virtualPath = virtualPath;
        meta.physicalPath = physicalPath.empty() ? virtualPath : physicalPath;
        meta.lastModified = std::chrono::system_clock::now();

        m_registry[handle] = meta;
        m_pathToHandle[virtualPath] = handle;

        return handle;
    }

    bool HasAsset(AssetHandle handle) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_registry.find(handle) != m_registry.end();
    }

    const AssetMetadata* GetMetadata(AssetHandle handle) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(handle);
        if (it != m_registry.end()) {
            return &it->second;
        }
        return nullptr;
    }

    AssetHandle GetHandleFromPath(const std::string& virtualPath) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_pathToHandle.find(virtualPath);
        if (it != m_pathToHandle.end()) {
            return it->second;
        }
        return 0;
    }

    template<typename T>
    void StoreAsset(AssetHandle handle, std::shared_ptr<T> asset) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (asset) {
            asset->SetHandle(handle);
            asset->SetStatus(AssetStatus::Ready);
        }
        m_loadedAssets[handle] = asset;
    }

    template<typename T>
    std::shared_ptr<T> GetAsset(AssetHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_loadedAssets.find(handle);
        if (it != m_loadedAssets.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    std::shared_ptr<T> GetAsset(const std::string& virtualPath) {
        AssetHandle handle = GetHandleFromPath(virtualPath);
        if (handle != 0) {
            return GetAsset<T>(handle);
        }
        return nullptr;
    }

    void UnloadAsset(AssetHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_loadedAssets.erase(handle);
    }

    void EnableHotReloading(AssetHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(handle);
        if (it != m_registry.end() && !it->second.physicalPath.empty()) {
            m_watcher.Watch(it->second.physicalPath, [this, handle](const std::string& path, FileAction action) {
                (void)path;
                if (action == FileAction::Modified) {
                    this->OnAssetFileModified(handle);
                }
            });
        }
    }

    void SetOnAssetReloadedCallback(std::function<void(AssetHandle)> callback) {
        m_onReloadCallback = callback;
    }

    void PollHotReload() {
        m_watcher.PollChanges();
    }

    void TriggerFileChangeManually(const std::string& physicalPath, FileAction action) {
        m_watcher.TriggerChangeManually(physicalPath, action);
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_registry.clear();
        m_pathToHandle.clear();
        m_loadedAssets.clear();
    }

private:
    void OnAssetFileModified(AssetHandle handle) {
        if (m_onReloadCallback) {
            m_onReloadCallback(handle);
        }
    }

    mutable std::mutex m_mutex;
    std::unordered_map<AssetHandle, AssetMetadata> m_registry;
    std::unordered_map<std::string, AssetHandle> m_pathToHandle;
    std::unordered_map<AssetHandle, std::shared_ptr<IAsset>> m_loadedAssets;

    FileWatcher m_watcher;
    std::function<void(AssetHandle)> m_onReloadCallback;
};

} // namespace starlight::assets
