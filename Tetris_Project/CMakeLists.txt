cmake_minimum_required(VERSION 3.20)
project(Tetris_Project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Dependencies Location
set(DEPS_DIR "d:/deps")
set(STARLIGHT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../StarlightEngine")

# Industrial Build Options
if(MSVC)
    add_compile_options(/W4 /WX /MP /Zi /bigobj /utf-8)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DGLM_ENABLE_EXPERIMENTAL)
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

add_executable(Tetris_Project ${SOURCES})

# Include Paths
target_include_directories(Tetris_Project PRIVATE 
    "${STARLIGHT_DIR}/include"
    "${STARLIGHT_DIR}/src"
    "${STARLIGHT_DIR}/thirdparty"
    "${STARLIGHT_DIR}/thirdparty/imgui"
    "${STARLIGHT_DIR}/thirdparty/wicked/core"
    "${STARLIGHT_DIR}/thirdparty/pl_mpeg"
    "${STARLIGHT_DIR}/thirdparty/nlohmann"
    "${DEPS_DIR}/glm-1.0.3"
    "${DEPS_DIR}/entt-3.16.0/src"
    "${DEPS_DIR}/sol2-3.5.0/include"
    "${DEPS_DIR}/lua-5.4.7"
    "${DEPS_DIR}/SDL-release-2.30.11/include"
    "${DEPS_DIR}/JoltPhysics-5.5.0"
)

# Link StarlightCore and its static dependencies
target_link_directories(Tetris_Project PRIVATE 
    "${STARLIGHT_DIR}/build/Release"
    "${STARLIGHT_DIR}/build/sdl2/Release"
    "${STARLIGHT_DIR}/build/jolt/Release"
    "${STARLIGHT_DIR}/build/_deps/meshoptimizer-build/Release"
    "${STARLIGHT_DIR}/build/_deps/ozz_animation-build/src/animation/runtime/Release"
    "${STARLIGHT_DIR}/build/_deps/ozz_animation-build/src/base/Release"
    "${STARLIGHT_DIR}/build/_deps/ozz_animation-build/src/geometry/runtime/Release"
)

target_link_libraries(Tetris_Project PRIVATE 
    StarlightCore
    glad
    lua
    meshoptimizer
    Jolt
    ozz_animation_r
    ozz_base_r
    ozz_geometry_r
    SDL2-static
    SDL2main
    kernel32 user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8
)

# Asset Sync Post-Build
add_custom_command(TARGET Tetris_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:Tetris_Project>/assets"
)
