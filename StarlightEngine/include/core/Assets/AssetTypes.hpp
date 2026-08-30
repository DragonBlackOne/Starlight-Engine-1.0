#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace starlight::assets {

using AssetHandle = uint64_t;

enum class AssetType {
    Unknown = 0,
    Texture,
    Mesh3D,
    Audio,
    Shader,
    Material,
    Scene,
    Script,
    Font
};

enum class AssetStatus {
    Unloaded = 0,
    Loading,
    Ready,
    Failed
};

inline const char* AssetTypeToString(AssetType type) {
    switch (type) {
        case AssetType::Texture:  return "Texture";
        case AssetType::Mesh3D:   return "Mesh3D";
        case AssetType::Audio:    return "Audio";
        case AssetType::Shader:   return "Shader";
        case AssetType::Material: return "Material";
        case AssetType::Scene:    return "Scene";
        case AssetType::Script:   return "Script";
        case AssetType::Font:     return "Font";
        default:                  return "Unknown";
    }
}

struct AssetMetadata {
    AssetHandle handle = 0;
    AssetType type = AssetType::Unknown;
    std::string virtualPath;
    std::string physicalPath;
    std::chrono::system_clock::time_point lastModified;
    std::vector<AssetHandle> dependencies;
    bool isMemoryOnly = false;
};

} // namespace starlight::assets
