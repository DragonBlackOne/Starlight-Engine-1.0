#include "DecalSystem.hpp"
#include "Log.hpp"
#include <algorithm>

namespace starlight {

DecalSystem::DecalSystem() {
    m_decals.reserve(256);
}

bool DecalSystem::OnInitialize([[maybe_unused]] const EngineContext& context) {
    (void)context;
    Log::Info("DecalSystem: Initialized with capacity of {} active decals.", m_maxDecals);
    return true;
}

void DecalSystem::OnUpdate(float dt) {
    for (auto& decal : m_decals) {
        if (!decal.active)
            continue;

        decal.lifetime -= dt;
        if (decal.lifetime <= 0.0f) {
            decal.active = false;
            decal.opacity = 0.0f;
        } else {
            // Smooth fade-out in final 25% of lifetime
            float fadeStart = decal.maxLifetime * 0.25f;
            if (decal.lifetime < fadeStart) {
                decal.opacity = std::clamp(decal.lifetime / fadeStart, 0.0f, 1.0f);
            } else {
                decal.opacity = 1.0f;
            }
        }
    }

    // Clean up inactive decals if vector grows too large
    if (m_decals.size() > m_maxDecals) {
        m_decals.erase(
            std::remove_if(m_decals.begin(), m_decals.end(), [](const Decal& d) { return !d.active; }),
            m_decals.end()
        );
    }
}

void DecalSystem::OnShutdown() {
    m_decals.clear();
    Log::Info("DecalSystem: Shutdown.");
}

uint32_t DecalSystem::SpawnDecal(const glm::vec3& position,
                                 const glm::quat& rotation,
                                 const glm::vec3& extents,
                                 const std::string& texturePath,
                                 float lifetime,
                                 const glm::vec4& color,
                                 int blendMode) {
    // If capacity reached, recycle oldest inactive or first active
    if (m_decals.size() >= m_maxDecals) {
        auto it = std::find_if(m_decals.begin(), m_decals.end(), [](const Decal& d) { return !d.active; });
        if (it != m_decals.end()) {
            it->id = m_nextId++;
            it->position = position;
            it->rotation = rotation;
            it->extents = extents;
            it->texturePath = texturePath;
            it->color = color;
            it->lifetime = std::max(0.1f, lifetime);
            it->maxLifetime = it->lifetime;
            it->opacity = 1.0f;
            it->blendMode = blendMode;
            it->active = true;
            return it->id;
        } else if (!m_decals.empty()) {
            // Overwrite oldest
            auto& oldest = m_decals.front();
            oldest.id = m_nextId++;
            oldest.position = position;
            oldest.rotation = rotation;
            oldest.extents = extents;
            oldest.texturePath = texturePath;
            oldest.color = color;
            oldest.lifetime = std::max(0.1f, lifetime);
            oldest.maxLifetime = oldest.lifetime;
            oldest.opacity = 1.0f;
            oldest.blendMode = blendMode;
            oldest.active = true;
            return oldest.id;
        }
    }

    Decal decal;
    decal.id = m_nextId++;
    decal.position = position;
    decal.rotation = rotation;
    decal.extents = extents;
    decal.texturePath = texturePath;
    decal.color = color;
    decal.lifetime = std::max(0.1f, lifetime);
    decal.maxLifetime = decal.lifetime;
    decal.opacity = 1.0f;
    decal.blendMode = blendMode;
    decal.active = true;

    m_decals.push_back(decal);
    return decal.id;
}

void DecalSystem::ClearAll() {
    m_decals.clear();
}

size_t DecalSystem::GetActiveDecalCount() const {
    size_t count = 0;
    for (const auto& d : m_decals) {
        if (d.active) count++;
    }
    return count;
}

} // namespace starlight
