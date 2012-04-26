# Use of this file is subject to license terms as set forth in the LICENSE file found in the root directory of the project.

cmake_minimum_required(VERSION 2.6)
project(node)

if(USE_GCOV)
  set(CMAKE_BUILD_TYPE "Debug")

  # Set global c and c++ flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

  # Link flags used for creating executables
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov -fprofile-arcs")

  # Link flags used for creating shared libraries
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcov -profile-arcs")
endif()

#
# options
#

find_package(PythonInterp 2 REQUIRED)

# NB. The code is not guaranteed to build if WEBOS is False.
set(WEBOS True CACHE INTERNAL "build for webOS")

option(SHARED_V8 "use system shared V8 library")
option(SHARED_LIBEV "use system shared libev library")
option(SHARED_CARES "use system shared c-ares library")
option(V8_SNAPSHOT "turn on snapshot when building stock v8")
option(V8_OPROFILE "Add oprofile support")
option(V8_GDBJIT "add gdbjit support")
option(DTRACE "build with DTrace (experimental)")

# cmake policies to get rid of some warnings
cmake_policy(SET CMP0009 NEW) # GLOB_RECURSE should no follow symlinks

# generic cmake configuration
include("cmake/configure.cmake")

# find and configure libs
include("cmake/libs.cmake")

# setup node build targets
include("cmake/node_build.cmake")

# setup v8 build targets
include("cmake/v8_build.cmake")

# docs
## might want to move this to doc/CMakeLists.txt
include("cmake/docs.cmake")

# tests
enable_testing()
include(CTest)
add_subdirectory("test/")

# package
include("cmake/package.cmake")

if(WEBOS)
  # Make sure the build system's version of this component matches what we're building.
  # The intent is for it to pass in NODEJS_COMPONENT_VERSION via the CMake command line.
  # This can't appear any earlier in the code because node_version_string isn't set until
  # cmake/package.cmake is invoked.
  if (DEFINED NODEJS_COMPONENT_VERSION)
    if (NOT (node_version_string STREQUAL NODEJS_COMPONENT_VERSION))
      message(FATAL_ERROR "Component version from build system (${NODEJS_COMPONENT_VERSION}) != configured version (${node_version_string})")
    endif ()
  endif ()


  # For some unknown reason, the upstream code doesn't process nodejs.pc.in . This can't go into cmake/configure.cmake
  # because node_version_string isn't set until cmake/package.cmake is invoked.
  if(NOT DEFINED ENV{PKG_CONFIG_PATH})
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
  endif()

  set(VERSION "${node_version_string}")
  configure_file(${PROJECT_SOURCE_DIR}/tools/nodejs.pc.in
                 ${PROJECT_BINARY_DIR}/tools/nodejs.pc @ONLY)
  install(FILES "${PROJECT_BINARY_DIR}/tools/nodejs.pc" DESTINATION "lib/pkgconfig")  
endif()

#
# Final build configuration output
#

message("** Build Summary **")
message("  Version:            ${node_version_string}")
message("  Prefix:             ${PREFIX}")
message("  Build Type:         ${CMAKE_BUILD_TYPE}")
message("  Architecture:       ${CMAKE_SYSTEM_PROCESSOR}")
message("  Platform:           ${node_platform}")

if(SHARED_V8)
  message("  V8:                 ${V8_LIBRARY_PATH}")
else()
  message("  V8:                 deps/v8/libv8.a")
endif()

if(SHARED_libev)
  message("  libev:              ${LIBEV_LIBRARY}")
else()
  message("  libev:              deps/libev/libev.a")
endif()

if(SHARED_CARES)
  message("  libc-ares:          ${LIBCARES_LIBRARY}")
else()
  message("  libc-ares:          deps/c-ares/libcares.a")
endif()

message("  RT library:         ${RT}")
message("  DL library:         ${DL}")

if(${OPENSSL_FOUND} MATCHES TRUE)
  message("  OpenSSL:            ${OPENSSL_LIBRARIES}")
endif()

if(USE_GCOV)
  message("  gcov:               enabled")
endif()

message("  CCFLAGS:            ${CCFLAGS}")
message("  CPPFLAGS:           ${CPPFLAGS}")

if(WEBOS)
  if(WEBOS_DEVICE)
    message("  webOS target:       device")
  elseif(WEBOS_EMULATOR)
    message("  webOS target:       emulator")
  else()
    message("  webOS target:       desktop")
  endif()
  message("  core OS:            ${TARGET_CORE_OS}")
endif()
