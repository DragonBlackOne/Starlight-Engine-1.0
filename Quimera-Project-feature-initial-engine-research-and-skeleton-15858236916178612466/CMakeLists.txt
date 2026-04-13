cmake_minimum_required(VERSION 3.15)
project(VamosEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Forçar /MD (Dynamic Runtime) - consistente com vcpkg
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Flags MSVC
if(MSVC)
    add_compile_options(/utf-8 /bigobj)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

# GLM
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# ================================================================
# ==================== THIRDPARTY LIBRARIES =======================
# ================================================================

# ---- Lua 5.4 ----
set(LUA_DIR ${CMAKE_SOURCE_DIR}/thirdparty/lua-5.4.7/src)
file(GLOB LUA_SOURCES "${LUA_DIR}/*.c")
list(REMOVE_ITEM LUA_SOURCES "${LUA_DIR}/lua.c" "${LUA_DIR}/luac.c")
add_library(lua54 STATIC ${LUA_SOURCES})
target_include_directories(lua54 PUBLIC ${LUA_DIR})
set_target_properties(lua54 PROPERTIES LINKER_LANGUAGE C)

# ---- Jolt Physics (3D) ----
set(JPH_USE_SSE ON CACHE BOOL "")
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "")
set(JOLT_PHYSICS_DIR ${CMAKE_SOURCE_DIR}/thirdparty/JoltPhysics)
include_directories(${JOLT_PHYSICS_DIR})
add_subdirectory(${JOLT_PHYSICS_DIR}/Build JoltBuild)

# ---- Box2D v3 (2D Physics) ----
set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(BOX2D_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
set(BOX2D_VALIDATE OFF CACHE BOOL "" FORCE)
set(BOX2D_UNIT_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/box2d)

# ---- Recast/Detour (Navigation/Pathfinding) ----
file(GLOB RECAST_SOURCES "thirdparty/recastnavigation/Recast/Source/*.cpp")
file(GLOB DETOUR_SOURCES "thirdparty/recastnavigation/Detour/Source/*.cpp")
file(GLOB DETOURCROWD_SOURCES "thirdparty/recastnavigation/DetourCrowd/Source/*.cpp")
add_library(RecastDetour STATIC ${RECAST_SOURCES} ${DETOUR_SOURCES} ${DETOURCROWD_SOURCES})
target_include_directories(RecastDetour PUBLIC
    thirdparty/recastnavigation/Recast/Include
    thirdparty/recastnavigation/Detour/Include
    thirdparty/recastnavigation/DetourCrowd/Include
)

# ---- ENet (Networking) ----
file(GLOB ENET_SOURCES "thirdparty/enet/*.c")
# Remover unix.c no Windows
list(REMOVE_ITEM ENET_SOURCES "${CMAKE_SOURCE_DIR}/thirdparty/enet/unix.c")
add_library(enet STATIC ${ENET_SOURCES})
target_include_directories(enet PUBLIC thirdparty/enet/include)
target_link_libraries(enet ws2_32 winmm)
set_target_properties(enet PROPERTIES LINKER_LANGUAGE C)

# ---- yaml-cpp ----
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/yaml-cpp)

# ---- Gainput (Input System) ----
file(GLOB_RECURSE GAINPUT_SOURCES "thirdparty/gainput/lib/source/gainput/*.cpp")
# Remover fontes Mac-specific
list(FILTER GAINPUT_SOURCES EXCLUDE REGEX ".*Mac\\.cpp$")
list(FILTER GAINPUT_SOURCES EXCLUDE REGEX ".*Linux\\.cpp$")
list(FILTER GAINPUT_SOURCES EXCLUDE REGEX ".*Android\\.cpp$")
list(FILTER GAINPUT_SOURCES EXCLUDE REGEX ".*Ios\\.cpp$")
add_library(gainput STATIC ${GAINPUT_SOURCES})
target_include_directories(gainput PUBLIC thirdparty/gainput/lib/include)
target_compile_definitions(gainput PRIVATE GAINPUT_PLATFORM_WIN)
target_link_libraries(gainput xinput)

# ---- Tracy (Profiling) - Client apenas ----
add_library(tracy STATIC thirdparty/tracy/public/TracyClient.cpp)
target_include_directories(tracy PUBLIC thirdparty/tracy/public)
# TRACY_ENABLE ativado apenas em Debug/RelWithDebInfo
target_compile_definitions(tracy PUBLIC
    $<$<NOT:$<CONFIG:Release>>:TRACY_ENABLE>
)

# ---- meshoptimizer (MIT) - Mesh LOD/optimization ----
add_subdirectory(thirdparty/meshoptimizer)

# ---- effekseer (MIT) - Particles Engine ----
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
set(USE_DSOUND OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/effekseer)

# ---- BehaviorTree.CPP (MIT) ----
set(BTCPP_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BTCPP_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BTCPP_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BTCPP_GROOT_INTERFACE OFF CACHE BOOL "" FORCE)
set(BTCPP_SQLITE_LOGGING OFF CACHE BOOL "" FORCE)
set(BTCPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/behaviortree)

# ---- ozz-animation (MIT) ----
set(ozz_build_samples OFF CACHE BOOL "" FORCE)
set(ozz_build_howtos OFF CACHE BOOL "" FORCE)
set(ozz_build_tests OFF CACHE BOOL "" FORCE)
set(ozz_build_tools OFF CACHE BOOL "" FORCE)
set(ozz_build_msvc_rt_dll ON CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/ozz-animation)


# ================================================================
# =================== VCPKG PACKAGES =============================
# ================================================================
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(bgfx CONFIG REQUIRED)
find_package(PhysFS CONFIG REQUIRED)
find_package(RmlUi CONFIG REQUIRED)
find_package(basisu CONFIG REQUIRED)

# ================================================================
# =================== INCLUDE DIRECTORIES =========================
# ================================================================
include_directories(include)
include_directories(thirdparty)
include_directories(thirdparty/entt/src)
include_directories(thirdparty/spdlog/include)
include_directories(thirdparty/imgui)
include_directories(thirdparty/imgui/backends)
include_directories(thirdparty/sol2/include)
include_directories(${LUA_DIR})

# Header-only libs (Round 1)
include_directories(thirdparty/json/single_include)       # nlohmann/json
include_directories(thirdparty/cereal/include)              # cereal
include_directories(thirdparty/fontstash/src)               # fontstash
include_directories(thirdparty/cpp-httplib)                 # cpp-httplib
include_directories(thirdparty/tracy/public)                # Tracy
include_directories(thirdparty/recastnavigation/Recast/Include)
include_directories(thirdparty/recastnavigation/Detour/Include)
include_directories(thirdparty/recastnavigation/DetourCrowd/Include)
include_directories(thirdparty/box2d/include)
include_directories(thirdparty/gainput/lib/include)
include_directories(thirdparty/enet/include)

# Header-only libs (Round 2 - Componentes de engines)
include_directories(thirdparty)                              # dr_libs, FastNoiseLite, etc.
include_directories(thirdparty/meshoptimizer/src)             # meshoptimizer
include_directories(thirdparty/tinygltf)                     # tinygltf (glTF 2.0)
include_directories(thirdparty/tinyobjloader)                # tinyobjloader (OBJ)
include_directories(thirdparty/taskflow)                     # Taskflow (task parallelism)
include_directories(thirdparty/lz4/lib)                      # LZ4 (compressão)
include_directories(thirdparty/pl_mpeg)                      # pl_mpeg (vídeo)
include_directories(thirdparty/microui/src)                  # microui (in-game UI)
include_directories(thirdparty/ImGuizmo)                     # ImGuizmo (gizmos 3D)
include_directories(thirdparty/simpleini)                    # SimpleIni (INI files)
include_directories(thirdparty/ozz-animation/include)        # ozz-animation
include_directories(thirdparty/effekseer/Dev/Cpp/Effekseer)  # Effekseer Core
include_directories(thirdparty/effekseer/Dev/Cpp/EffekseerRendererGL) # Effekseer GL

# ================================================================
# ===================== ENGINE SOURCES ============================
# ================================================================
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
list(APPEND SOURCES "thirdparty/wicked/core/wiJobSystem.cpp")
set(IMGUI_SOURCES
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/ImGuizmo/ImGuizmo.cpp
    thirdparty/ImGuizmo/ImCurveEdit.cpp
    thirdparty/ImGuizmo/ImGradient.cpp
    thirdparty/ImGuizmo/ImSequencer.cpp
    thirdparty/ImGuizmo/GraphEditor.cpp
)

add_executable(VamosEngine ${SOURCES} ${IMGUI_SOURCES})

# ================================================================
# ====================== LINK LIBRARIES ==========================
# ================================================================
target_link_libraries(VamosEngine
    # Rendering
    glfw
    GLEW::GLEW
    glm::glm
    opengl32.lib

    # Rendering Core (Basic)
    # Jolt
    # box2d
    assimp::assimp
    # lua54
    # RecastDetour
    # enet

    # Heavy Engines (Phase 3)
    # bgfx::bx
    # bgfx::bimg
    # bgfx::bimg_decode
    # bgfx::bgfx
    # PhysFS::PhysFS
    # RmlUi::RmlUi
    # basisu::basisu_encoder
    # Effekseer
    # EffekseerRendererGL

    # Input
    gainput

    # Config
    yaml-cpp

    # Profiling
    tracy

    # Mesh Optimization
    meshoptimizer

    # AI & Animation
    # behaviortree_cpp
    # ozz_animation
    # ozz_base
    
    # System (Windows)
    ws2_32
    winmm
)

# ================================================================
# ====================== POST BUILD ==============================
# ================================================================
add_custom_command(TARGET VamosEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:VamosEngine>/assets)

add_custom_command(TARGET VamosEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/vcpkg_tool/installed/x64-windows/bin"
    $<TARGET_FILE_DIR:VamosEngine>
)
