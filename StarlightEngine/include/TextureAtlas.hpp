#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace starlight {

    struct AtlasRegion {
        std::string name;
        float u = 0.0f, v = 0.0f;
        float u2 = 1.0f, v2 = 1.0f;
        float width = 0.0f, height = 0.0f;
    };

    class TextureAtlas {
    public:
        TextureAtlas() = default;
        ~TextureAtlas() = default;

        void LoadGrid(uint32_t textureID, int columns, int rows, int totalFrames = -1);
        void AddRegion(const std::string& name, float u, float v, float u2, float v2);
        void AddRegion(const std::string& name, int col, int row, int gridCols, int gridRows);

        const AtlasRegion* GetRegion(const std::string& name) const;
        const AtlasRegion* GetFrame(int index) const;
        int GetFrameCount() const { return (int)m_regions.size(); }

        uint32_t GetTextureID() const { return m_textureID; }
        void SetTextureID(uint32_t id) { m_textureID = id; }

    private:
        uint32_t m_textureID = 0;
        std::vector<AtlasRegion> m_regions;
        std::unordered_map<std::string, int> m_nameMap;
    };

}
