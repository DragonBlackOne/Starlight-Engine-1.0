# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-src")
  file(MAKE_DIRECTORY "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-src")
endif()
file(MAKE_DIRECTORY
  "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-build"
  "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix"
  "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix/tmp"
  "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix/src/cgltf-populate-stamp"
  "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix/src"
  "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix/src/cgltf-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix/src/cgltf-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Projetos/Fusion ENGINE/TitanEngine/build_new/_deps/cgltf-subbuild/cgltf-populate-prefix/src/cgltf-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
