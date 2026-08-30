#pragma once
#include "AssetTypes.hpp"
#include <memory>

namespace starlight::assets {

class IAsset {
public:
    virtual ~IAsset() = default;

    AssetHandle GetHandle() const { return m_handle; }
    void SetHandle(AssetHandle handle) { m_handle = handle; }

    AssetType GetType() const { return m_type; }
    void SetType(AssetType type) { m_type = type; }

    AssetStatus GetStatus() const { return m_status; }
    void SetStatus(AssetStatus status) { m_status = status; }

    bool IsReady() const { return m_status == AssetStatus::Ready; }

protected:
    AssetHandle m_handle = 0;
    AssetType m_type = AssetType::Unknown;
    AssetStatus m_status = AssetStatus::Unloaded;
};

// ---------------------------------------------------------------------------
// Common Specialized Asset Wrappers
// ---------------------------------------------------------------------------

class TextAsset : public IAsset {
public:
    TextAsset() { m_type = AssetType::Script; }
    explicit TextAsset(std::string text) : m_text(std::move(text)) {
        m_type = AssetType::Script;
        m_status = AssetStatus::Ready;
    }

    const std::string& GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }

private:
    std::string m_text;
};

class BinaryAsset : public IAsset {
public:
    BinaryAsset() { m_type = AssetType::Unknown; }
    explicit BinaryAsset(std::vector<uint8_t> data) : m_data(std::move(data)) {
        m_status = AssetStatus::Ready;
    }

    const std::vector<uint8_t>& GetData() const { return m_data; }
    size_t GetSize() const { return m_data.size(); }

private:
    std::vector<uint8_t> m_data;
};

} // namespace starlight::assets
