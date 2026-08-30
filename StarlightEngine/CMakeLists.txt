cmake_minimum_required(VERSION 3.20)
project(StarlightEngine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
# Semantic Versioning
set(STARLIGHT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(STARLIGHT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(STARLIGHT_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(STARLIGHT_VERSION_STRING "${STARLIGHT_VERSION_MAJOR}.${STARLIGHT_VERSION_MINOR}.${STARLIGHT_VERSION_PATCH}")

# Generate version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/StarlightVersion.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/StarlightVersion.hpp
    @ONLY
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

set(STARLIGHT_SDK_ROOT ${CMAKE_CURRENT_LIST_DIR})

# --- Open Source Dependencies ---
include(FetchContent)
FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG        v1.1
    GIT_SHALLOW    TRUE
)
set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(meshoptimizer)

FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
    GIT_TAG        v1.15
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cgltf)

FetchContent_Declare(
    ozz_animation
    GIT_REPOSITORY https://github.com/guillaumeblanc/ozz-animation.git
    GIT_TAG        0.16.0
    GIT_SHALLOW    TRUE
)
set(OZZ_ANIMATION_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(OZZ_ANIMATION_BUILD_HOWTOS OFF CACHE BOOL "" FORCE)
set(OZZ_ANIMATION_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ozz_build_tests OFF CACHE BOOL "" FORCE)
set(ozz_build_msvc_rt_dll ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ozz_animation)

set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG        VER-2-13-0
    GIT_SHALLOW    TRUE
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(freetype)

# Expose Freetype variables for RmlUi configuration
set(FREETYPE_INCLUDE_DIRS "${freetype_SOURCE_DIR}/include" "${freetype_BINARY_DIR}/include" CACHE PATH "" FORCE)
set(FREETYPE_LIBRARIES "${freetype_BINARY_DIR}/$<CONFIG>/freetype$<$<CONFIG:Debug>:d>.lib" CACHE STRING "" FORCE)
set(FREETYPE_FOUND TRUE CACHE BOOL "" FORCE)

FetchContent_Declare(
    rmlui
    GIT_REPOSITORY https://github.com/mikke89/RmlUi.git
    GIT_TAG        5.0
    GIT_SHALLOW    TRUE
)
set(RMLUI_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(RMLUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(rmlui)

FetchContent_Declare(
    imgui_node_editor
    GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(imgui_node_editor)
if(NOT imgui_node_editor_POPULATED)
    FetchContent_Populate(imgui_node_editor)
endif()

add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

# Dependency root. Resolution order:
#   1. CMake cache variable: -DSTARLIGHT_DEPS_DIR=...
#   2. Environment variable: STARLIGHT_DEPS_DIR
#   3. Default: d:/deps (matches CI workflow)
if(DEFINED STARLIGHT_DEPS_DIR)
    message(STATUS "StarlightEngine: DEPS_DIR from cache = ${STARLIGHT_DEPS_DIR}")
elseif(DEFINED ENV{STARLIGHT_DEPS_DIR})
    set(STARLIGHT_DEPS_DIR "$ENV{STARLIGHT_DEPS_DIR}")
    message(STATUS "StarlightEngine: DEPS_DIR from env = ${STARLIGHT_DEPS_DIR}")
else()
    set(STARLIGHT_DEPS_DIR "d:/deps")
    message(STATUS "StarlightEngine: DEPS_DIR default = ${STARLIGHT_DEPS_DIR}")
endif()
set(DEPS_DIR "${STARLIGHT_DEPS_DIR}")

# GLM, EnTT, Sol2, Lua
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${DEPS_DIR}/glm-1.0.3)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${DEPS_DIR}/entt-3.16.0/src)
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE ${DEPS_DIR}/sol2-3.5.0/include)

file(GLOB LUA_SOURCES "${DEPS_DIR}/lua-5.4.7/*.c")
list(REMOVE_ITEM LUA_SOURCES 
    "${DEPS_DIR}/lua-5.4.7/lua.c" "${DEPS_DIR}/lua-5.4.7/luac.c" 
    "${DEPS_DIR}/lua-5.4.7/onelua.c" "${DEPS_DIR}/lua-5.4.7/ltests.c"
)
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${DEPS_DIR}/lua-5.4.7)

# SDL2 & Jolt
add_subdirectory(${DEPS_DIR}/SDL-release-2.30.11 ${CMAKE_BINARY_DIR}/sdl2 EXCLUDE_FROM_ALL)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(CPP_RTTI_ENABLED ON CACHE BOOL "" FORCE)
add_subdirectory(${DEPS_DIR}/JoltPhysics-5.5.0/Build ${CMAKE_BINARY_DIR}/jolt EXCLUDE_FROM_ALL)

# --- StarlightEngine CORE LIBRARY ---
set(STARLIGHT_INCLUDE_DIR ${STARLIGHT_SDK_ROOT}/include)
set(STARLIGHT_SRC_DIR ${STARLIGHT_SDK_ROOT}/src)

add_library(glad STATIC ${STARLIGHT_INCLUDE_DIR}/glad/src/glad.cpp)
target_include_directories(glad PUBLIC ${STARLIGHT_INCLUDE_DIR}/glad/include)

file(GLOB_RECURSE CORE_SOURCES 
    "${STARLIGHT_SRC_DIR}/ai/*.cpp"
    "${STARLIGHT_SRC_DIR}/audio/*.cpp"
    "${STARLIGHT_SRC_DIR}/core/*.cpp"
    "${STARLIGHT_SRC_DIR}/ecs/*.cpp"
    "${STARLIGHT_SRC_DIR}/gui/*.cpp"
    "${STARLIGHT_SRC_DIR}/network/*.cpp"
    "${STARLIGHT_SRC_DIR}/physics/*.cpp"
    "${STARLIGHT_SRC_DIR}/renderer/*.cpp"
    "${STARLIGHT_SDK_ROOT}/thirdparty/wicked/core/wiJobSystem.cpp"
)

file(GLOB IMGUI_SOURCES "${STARLIGHT_SDK_ROOT}/thirdparty/imgui/*.cpp")
set(MINIZ_SOURCES "${STARLIGHT_SDK_ROOT}/thirdparty/miniz.cpp")
set(IMGUI_NODE_EDITOR_SOURCES
    "${imgui_node_editor_SOURCE_DIR}/imgui_node_editor.cpp"
    "${imgui_node_editor_SOURCE_DIR}/imgui_node_editor_api.cpp"
    "${imgui_node_editor_SOURCE_DIR}/imgui_canvas.cpp"
    "${imgui_node_editor_SOURCE_DIR}/crude_json.cpp"
)

# Disable code analysis on third-party source files and suppress warnings
set_source_files_properties(${IMGUI_SOURCES} ${MINIZ_SOURCES} ${IMGUI_NODE_EDITOR_SOURCES} PROPERTIES COMPILE_FLAGS "/analyze- /W0 /WX-")
set_source_files_properties(${MINIZ_SOURCES} ${IMGUI_NODE_EDITOR_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties(${IMGUI_NODE_EDITOR_SOURCES} PROPERTIES COMPILE_DEFINITIONS "IMGUI_NODE_EDITOR_BUILD")

add_library(StarlightCore STATIC ${CORE_SOURCES} ${IMGUI_SOURCES} ${MINIZ_SOURCES} ${IMGUI_NODE_EDITOR_SOURCES})
target_include_directories(StarlightCore PUBLIC 
    ${STARLIGHT_INCLUDE_DIR}
    ${STARLIGHT_SRC_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${STARLIGHT_SDK_ROOT}/thirdparty
    ${STARLIGHT_SDK_ROOT}/thirdparty/imgui
    ${STARLIGHT_SDK_ROOT}/thirdparty/pl_mpeg
    ${STARLIGHT_SDK_ROOT}/thirdparty/nlohmann
    ${STARLIGHT_SDK_ROOT}/thirdparty/wicked/core
    ${imgui_node_editor_SOURCE_DIR}
)
target_link_libraries(StarlightCore PUBLIC 
    SDL2::SDL2main SDL2::SDL2-static
    glad Jolt glm entt sol2 lua meshoptimizer cgltf 
    ozz_animation ozz_base ozz_geometry
    RmlCore
)
target_compile_definitions(StarlightCore PUBLIC SDL_MAIN_HANDLED UNICODE _UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL IMGUI_DEFINE_MATH_OPERATORS)
# Strict Warnings Enabled for Industrial Audit
target_compile_options(StarlightCore PUBLIC /W4 /bigobj)

if(MSVC)
    # /wd4005: imgui_impl_opengl3_loader.h macro redefinitions (conflict with glad)
    # /wd5321: sol2/inheritance.hpp UTF-8 multibyte encoding extension
    target_compile_options(StarlightCore PRIVATE /MP /EHsc /FS /utf-8 /wd4005 /wd5321)
    # Treat warnings as errors in Release (quality gate)
    target_compile_options(StarlightCore PRIVATE $<$<CONFIG:Release>:/WX>)
endif()

# Precompiled Headers (accelerates rebuilds ~2x)
target_precompile_headers(StarlightCore PUBLIC
    ${STARLIGHT_SRC_DIR}/StarlightCorePCH.h
)

# Unity / Jumbo Build (merge TUs for faster compilation)
# DISABLED: pre-existing duplicate type definitions across headers
# set_target_properties(StarlightCore PROPERTIES UNITY_BUILD ON)

# --- SHOWCASE TARGET ---
add_executable(StarlightShowcase 
    ${STARLIGHT_SDK_ROOT}/examples/StarlightShowcase/main.cpp
)
target_link_libraries(StarlightShowcase PRIVATE StarlightCore)
if(MSVC)
    # /wd4100: [[maybe_unused]] params (argc/argv)
    # /wd5321: sol2/inheritance.hpp UTF-8 multibyte encoding extension
    target_compile_options(StarlightShowcase PRIVATE /wd4100 /wd5321)
endif()
add_custom_command(TARGET StarlightShowcase POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${STARLIGHT_SDK_ROOT}/assets"
    "$<TARGET_FILE_DIR:StarlightShowcase>/assets"
)

# --- TESTS (optional) ---
option(STARLIGHT_BUILD_TESTS "Build unit tests with GoogleTest" OFF)
if(STARLIGHT_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
endif()
