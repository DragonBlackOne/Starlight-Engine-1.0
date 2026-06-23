add_executable(StarlightTests
    test_main.cpp
    test_ArchitectureSystems.cpp
    test_FightingSystem.cpp
    test_NavigationSystem.cpp
    test_TextureAtlas.cpp
    test_Noise.cpp
    test_Easing.cpp
    test_Tween.cpp
    test_BehaviorTree.cpp
    test_SIMDMath.cpp
    test_SceneStack.cpp
    test_Memory.cpp
    test_EventSystem.cpp
    test_ConsoleSystem.cpp
    test_CameraSystem.cpp
    test_LODSystem.cpp
    test_ClothSystem.cpp
    test_Components.cpp
    test_SpriteAnimation.cpp
    test_HierarchySystem.cpp
    test_Log.cpp
    test_MeshDeformSystem.cpp
    test_VFSSystem.cpp
    test_PrefabAndRHI.cpp
    test_FrameworkFeatures.cpp
    test_DeferredGBuffer.cpp
    test_PhysicsCharacterVirtual.cpp
    test_PluginHotReload.cpp
    test_RagdollSystem.cpp
)

target_link_libraries(StarlightTests PRIVATE StarlightCore gtest_main)

target_include_directories(StarlightTests PRIVATE
    ${STARLIGHT_INCLUDE_DIR}
    ${STARLIGHT_SRC_DIR}
    ${STARLIGHT_SDK_ROOT}/thirdparty
)
