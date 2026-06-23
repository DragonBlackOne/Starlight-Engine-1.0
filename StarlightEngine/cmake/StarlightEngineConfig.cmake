# Starlight Engine SDK Config
# Loaded by consumer projects via:
#   include("${STARLIGHT_SDK_DIR}/cmake/StarlightEngineConfig.cmake")
# Defines:
#   STARLIGHT_SDK_DIR      - absolute path to the SDK root
#   STARLIGHT_INCLUDE_DIRS - list of public include directories
#   STARLIGHT_LIBRARIES    - list of libraries to link

set(STARLIGHT_SDK_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(STARLIGHT_INCLUDE_DIRS "${STARLIGHT_SDK_DIR}/include")
set(STARLIGHT_LIBRARIES StarlightCore)

message(STATUS "Starlight Engine SDK loaded from ${STARLIGHT_SDK_DIR}")
