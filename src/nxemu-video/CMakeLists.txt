cmake_minimum_required(VERSION 3.18)
project(nxemu-video LANGUAGES C CXX ASM)

# Windows (nxemu-video.vcxproj): same .cpp files + ProjectReference yuzu_video_core (full GPU).
# CMake: always link static `video_core` when that target exists (host builds). Android builds
# only video_core_frontend until full yuzu_video_core is enabled for NDK.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VERSION_HEADER_IN "${CMAKE_SOURCE_DIR}/src/nxemu-video/version.h.in")
set(VERSION_HEADER_OUT "${CMAKE_BINARY_DIR}/generated/nxemu-video/version.h")
include(generate_version_header)

set(NXEMU_VIDEO_SOURCES
    nxemu-video.cpp
    render_window.cpp
    video_manager.h
    video_settings.cpp
    video_settings.h
    video_settings_identifiers.h
    version.h
    render_window.h
)
if(ANDROID)
    list(APPEND NXEMU_VIDEO_SOURCES video_manager_android_stub.cpp)
else()
    list(APPEND NXEMU_VIDEO_SOURCES video_manager.cpp)
endif()

add_library(nxemu-video SHARED ${NXEMU_VIDEO_SOURCES})

target_include_directories(nxemu-video PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/yuzu_video_core
    ${CMAKE_SOURCE_DIR}/external/boost
)

target_link_libraries(nxemu-video PRIVATE video_core_frontend common)

if(TARGET video_core)
    target_link_libraries(nxemu-video PRIVATE video_core)
endif()

if(ANDROID)
    target_link_libraries(nxemu-video PRIVATE log)
endif()
