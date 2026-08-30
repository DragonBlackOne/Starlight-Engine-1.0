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
    test_CoreModule.cpp
    test_Module2.cpp
    test_Module3.cpp
    test_Module4.cpp
    test_Module5.cpp
    test_Module6.cpp
    test_Module7.cpp
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
    test_FuzzAndStress.cpp
    test_AutomationSystem.cpp
)

target_link_libraries(StarlightTests PRIVATE StarlightCore gtest_main)

target_include_directories(StarlightTests PRIVATE
    ${STARLIGHT_INCLUDE_DIR}
    ${STARLIGHT_SRC_DIR}
    ${STARLIGHT_SDK_ROOT}/thirdparty
)

add_custom_command(TARGET StarlightTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${STARLIGHT_SDK_ROOT}/assets"
    "$<TARGET_FILE_DIR:StarlightTests>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${STARLIGHT_SDK_ROOT}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

add_test(NAME EngineUnitTests COMMAND StarlightTests)
