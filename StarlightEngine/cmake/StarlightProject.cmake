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

    add_executable(${target_name} src/main.cpp)

    target_include_directories(${target_name} PRIVATE
        src
        "${_sdk}/include"
    )

    target_link_libraries(${target_name} PRIVATE
        StarlightCore
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W4 /WX /MP /EHsc /FS /utf-8 /Zi /bigobj /wd4005 /wd5321
        )
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            GLM_ENABLE_EXPERIMENTAL
            SOL_USING_CXX_LUA
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
