# StarlightProject.cmake
# Helper module for projects that consume the StarlightEngine SDK.
# Provides:
#   starlight_resolve_sdk_dir([var_name])
#       Locates the SDK relative to the calling CMakeLists.txt.
#       Default variable name: STARLIGHT_SDK_DIR
#       Sets ${var_name} to the absolute path or to <empty> on failure.
#
#   add_starlight_game(<target_name> [MODE_2D])
#       Creates an executable, wires include directories, link directories,
#       and libraries, and adds a POST_BUILD step that copies the local
#       `assets/` folder next to the produced binary.
#
# Assumes the project layout:
#   <ProjectRoot>/
#     CMakeLists.txt
#     src/main.cpp
#     assets/
#     ../StarlightEngine/        <-- SDK location
#
# The dependency root (glm, entt, sol2, lua, SDL2, JoltPhysics) is resolved
# from the cache variable STARLIGHT_DEPS_DIR. If not provided, it falls
# back to the environment variable of the same name, then to "d:/deps".

cmake_minimum_required(VERSION 3.20)

if(DEFINED STARLIGHT_DEPS_DIR)
    set(_starlight_deps_dir "${STARLIGHT_DEPS_DIR}")
elseif(DEFINED ENV{STARLIGHT_DEPS_DIR})
    set(_starlight_deps_dir "$ENV{STARLIGHT_DEPS_DIR}")
else()
    set(_starlight_deps_dir "d:/deps")
endif()

function(starlight_resolve_sdk_dir result_var)
    set(_sdk "${CMAKE_CURRENT_SOURCE_DIR}/../StarlightEngine")
    get_filename_component(_sdk "${_sdk}" ABSOLUTE)
    if(NOT EXISTS "${_sdk}/cmake/StarlightEngineConfig.cmake")
        message(FATAL_ERROR
            "StarlightEngine SDK not found at ${_sdk}. "
            "Place this project as a sibling of StarlightEngine/.")
    endif()
    set(${result_var} "${_sdk}" PARENT_SCOPE)
endfunction()

function(add_starlight_game target_name)
    set(_options  MODE_2D)
    set(_onevalue "")
    set(_multi    "")
    cmake_parse_arguments(_sg "${_options}" "${_onevalue}" "${_multi}" ${ARGN})

    starlight_resolve_sdk_dir(_sdk)
    set(_inc "${_sdk}/include")
    set(_lib "${_sdk}/build/$<CONFIG>")
    set(_deps "${_starlight_deps_dir}")

    add_executable(${target_name} src/main.cpp)

    target_include_directories(${target_name} PRIVATE
        src
        "${_inc}"
        "${_inc}/glad/include"
        "${_sdk}/src"
        "${_sdk}/thirdparty"
        "${_sdk}/thirdparty/imgui"
        "${_sdk}/thirdparty/wicked/core"
        "${_sdk}/thirdparty/pl_mpeg"
        "${_sdk}/thirdparty/nlohmann"
        "${_deps}/glm-1.0.3"
        "${_deps}/entt-3.16.0/src"
        "${_deps}/sol2-3.5.0/include"
        "${_deps}/lua-5.4.7"
        "${_deps}/SDL-release-2.30.11/include"
        "${_deps}/JoltPhysics-5.5.0"
    )

    target_link_directories(${target_name} PRIVATE
        "${_lib}"
        "${_sdk}/build/sdl2/$<CONFIG>"
        "${_sdk}/build/jolt/$<CONFIG>"
        "${_sdk}/build/_deps/meshoptimizer-build/$<CONFIG>"
        "${_sdk}/build/_deps/ozz_animation-build/src/animation/runtime/$<CONFIG>"
        "${_sdk}/build/_deps/ozz_animation-build/src/base/$<CONFIG>"
        "${_sdk}/build/_deps/ozz_animation-build/src/geometry/runtime/$<CONFIG>"
        "${_sdk}/build/_deps/rmlui-build/$<CONFIG>"
        "${_sdk}/build/_deps/freetype-build/$<CONFIG>"
    )

    target_link_libraries(${target_name} PRIVATE
        StarlightCore
        RmlCore
        $<$<CONFIG:Debug>:freetyped>$<$<NOT:$<CONFIG:Debug>>:freetype>
        glad
        lua
        meshoptimizer
        Jolt
        $<$<CONFIG:Debug>:ozz_animation_d>$<$<NOT:$<CONFIG:Debug>>:ozz_animation_r>
        $<$<CONFIG:Debug>:ozz_base_d>$<$<NOT:$<CONFIG:Debug>>:ozz_base_r>
        $<$<CONFIG:Debug>:ozz_geometry_d>$<$<NOT:$<CONFIG:Debug>>:ozz_geometry_r>
        $<$<CONFIG:Debug>:SDL2-staticd>$<$<NOT:$<CONFIG:Debug>>:SDL2-static>
        $<$<CONFIG:Debug>:SDL2maind>$<$<NOT:$<CONFIG:Debug>>:SDL2main>
        kernel32 user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W4 /WX /MP /EHsc /FS /utf-8 /Zi /bigobj /wd4005 /wd5321
            $<$<CONFIG:Release>:/WX>
        )
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            GLM_ENABLE_EXPERIMENTAL
        )
    endif()

    if(_sg_MODE_2D)
        target_compile_definitions(${target_name} PRIVATE STARLIGHT_MODE_2D=1)
    endif()

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${target_name}>/assets"
    )
endfunction()
