#include "TextureAtlas.hpp"

namespace starlight {

    void TextureAtlas::LoadGrid(uint32_t textureID, int columns, int rows, int totalFrames) {
        m_textureID = textureID;
        m_regions.clear();
        m_nameMap.clear();

        if (totalFrames < 0) totalFrames = columns * rows;

        float cellW = 1.0f / (float)columns;
        float cellH = 1.0f / (float)rows;

        int frame = 0;
        for (int row = 0; row < rows && frame < totalFrames; row++) {
            for (int col = 0; col < columns && frame < totalFrames; col++, frame++) {
                AtlasRegion region;
                region.name = "frame_" + std::to_string(frame);
                region.u = (float)col * cellW;
                region.v = (float)row * cellH;
                region.u2 = region.u + cellW;
                region.v2 = region.v + cellH;
                m_regions.push_back(region);
                m_nameMap[region.name] = frame;
            }
        }
    }

    void TextureAtlas::AddRegion(const std::string& name, float u, float v, float u2, float v2) {
        AtlasRegion region;
        region.name = name;
        region.u = u; region.v = v;
        region.u2 = u2; region.v2 = v2;
        int index = (int)m_regions.size();
        m_regions.push_back(region);
        m_nameMap[name] = index;
    }

    void TextureAtlas::AddRegion(const std::string& name, int col, int row, int gridCols, int gridRows) {
        float cellW = 1.0f / (float)gridCols;
        float cellH = 1.0f / (float)gridRows;
        AddRegion(name, (float)col * cellW, (float)row * cellH,
                  (float)(col + 1) * cellW, (float)(row + 1) * cellH);
    }

    const AtlasRegion* TextureAtlas::GetRegion(const std::string& name) const {
        auto it = m_nameMap.find(name);
        if (it != m_nameMap.end()) {
            return &m_regions[it->second];
        }
        return nullptr;
    }

    const AtlasRegion* TextureAtlas::GetFrame(int index) const {
        if (index >= 0 && index < (int)m_regions.size()) {
            return &m_regions[index];
        }
        return nullptr;
    }

}
