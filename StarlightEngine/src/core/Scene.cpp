#include "CoreMinimal.hpp"
#include "Components.hpp"
#include "SpriteAnimationComponent.hpp"
#include "AIComponent.hpp"

namespace starlight {

    Scene::Scene() {
        // Inicializa grupos contínuos no EnTT para otimização extrema de cache de CPU (L1/L2)
        // Evita cache misses reagrupando fisicamente os componentes em memória contígua.
        
        // 1. TransformComponent & PhysicsComponent (Física)
        m_registry.group<PhysicsComponent>(entt::get<TransformComponent>);

        // 2. TransformComponent & SpriteComponent (Sprites 2D)
        m_registry.group<SpriteComponent>(entt::get<TransformComponent>);

        // 3. TransformComponent & MeshComponent (Meshes 3D)
        m_registry.group<MeshComponent>(entt::get<TransformComponent>);

        // 4. SpriteAnimationComponent & SpriteComponent (Animações de Sprites)
        m_registry.group<SpriteAnimationComponent>(entt::get<SpriteComponent>);
    }

}
