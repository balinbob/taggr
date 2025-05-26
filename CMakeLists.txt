cmake_minimum_required(VERSION 3.14)
project(taggr VERSION 0.1.0 LANGUAGES CXX)

# ─── Suppress CMake developer warnings ─────────────────────────────────────────
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE CACHE BOOL "Suppress developer warnings" FORCE)

# ─── 1. Static libraries only ───────────────────────────────────────────────────
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# TagLib configuration (static only, no extras)
#set(TAGLIB_BUILD_SHARED OFF CACHE BOOL "Disable TagLib shared libs" FORCE)
#set(TAGLIB_BUILD_STATIC ON CACHE BOOL "Enable TagLib static libs" FORCE)
#set(TAGLIB_BUILD_EXAMPLES OFF CACHE BOOL "Disable TagLib examples" FORCE)
#set(TAGLIB_BUILD_TESTS OFF CACHE BOOL "Disable TagLib tests" FORCE)
#set(TAGLIB_USE_QT OFF CACHE BOOL "Disable TagLib Qt support" FORCE)
#set(TAGLIB_USE_ICU OFF CACHE BOOL "Disable TagLib ICU support" FORCE)
#set(TAGLIB_USE_ZLIB OFF CACHE BOOL "Disable TagLib zlib support" FORCE)

# ─── 2. FetchContent setup ────────────────────────────────────────────────────
include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.fetchcontent")

# TagLib from GitHub (bumped to latest stable release 2.0.2)
#FetchContent_Declare(
#$  taglib
#  GIT_REPOSITORY https://github.com/taglib/taglib.git
#  GIT_TAG        v2.0.2
#  GIT_SHALLOW    TRUE
#)
#FetchContent_MakeAvailable(taglib)

# CLI11 (header-only)
FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        main
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(CLI11)
add_library(cli11_includes INTERFACE)
target_include_directories(cli11_includes INTERFACE
    ${CLI11_SOURCE_DIR}/include
)
add_library(CLI11::CLI11 ALIAS cli11_includes)

# ─── 3. Patch glob header ──────────────────────────────────────────────────────
file(READ "${CMAKE_SOURCE_DIR}/include/glob/single_include/glob/glob.hpp" _hdr)
string(REPLACE "#ifdef _WIN32" "#if defined(_WIN32) && !defined(__MINGW32__)" _hdr "${_hdr}")
string(REPLACE "AND NOT" "&& !" _hdr "${_hdr}")
file(WRITE "${CMAKE_SOURCE_DIR}/include/glob/single_include/glob/glob.hpp" "${_hdr}")

# ─── 4. Project settings ──────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
"${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(taggr ${SOURCES})

target_include_directories(taggr PRIVATE
  ${TAGLIB_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/glob/single_include
)


target_link_libraries(taggr PRIVATE
  ${TAGLIB_LIBRARIES}
  CLI11::CLI11
)
