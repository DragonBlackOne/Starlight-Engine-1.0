#pragma once
#include "EngineSystem.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace starlight {

struct Decal {
    uint32_t id = 0;
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 extents{1.0f, 1.0f, 0.5f};
    std::string texturePath;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    float lifetime = 10.0f;
    float maxLifetime = 10.0f;
    float opacity = 1.0f;
    int blendMode = 0; // 0: Alpha, 1: Additive, 2: Multiply
    bool active = true;
};

class DecalSystem : public ISystem {
public:
    DecalSystem();
    ~DecalSystem() override = default;

    bool OnInitialize(const EngineContext& context) override;
    void OnUpdate(float dt) override;
    void OnShutdown() override;

    const char* GetName() const override { return "DecalSystem"; }
    bool IsMainThreadOnly() const override { return true; }
    int GetPriority() const override { return 45; }

    uint32_t SpawnDecal(const glm::vec3& position,
                        const glm::quat& rotation,
                        const glm::vec3& extents,
                        const std::string& texturePath,
                        float lifetime = 10.0f,
                        const glm::vec4& color = glm::vec4(1.0f),
                        int blendMode = 0);

    void ClearAll();
    size_t GetActiveDecalCount() const;
    const std::vector<Decal>& GetDecals() const { return m_decals; }

    void SetMaxDecals(size_t maxCount) { m_maxDecals = maxCount; }
    size_t GetMaxDecals() const { return m_maxDecals; }

private:
    std::vector<Decal> m_decals;
    size_t m_maxDecals = 256;
    uint32_t m_nextId = 1;
};

} // namespace starlight
