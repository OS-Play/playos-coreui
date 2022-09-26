cmake_minimum_required(VERSION 3.10.2)

project(coreui)

set(WAYLAND_PROTOCOLS ${CMAKE_SOURCE_DIR}/3rdparty/wayland-protocols)
add_definitions(-DWLR_USE_UNSTABLE -Werror)

find_package(PkgConfig)

if (CMAKE_CROSSCOMPILING)
if (NOT CMAKE_FIND_ROOT_PATH)
message(FATAL_ERROR "No rootfs found for cross compile, set CMAKE_FIND_ROOT_PATH please.")
endif()

set(MESON_CROSS_ARGS --cross-file ${CMAKE_SOURCE_DIR}/toolchain_files/meson_aarch64.txt)
include_directories(${CMAKE_FIND_ROOT_PATH}/usr/include)

pkg_check_modules(PIXMAN pixman-1 REQUIRED)

include_directories(${PIXMAN_INCLUDE_DIRS})
link_directories(BEFORE ${CMAKE_FIND_ROOT_PATH}/usr/lib)

add_link_options(--sysroot=${CMAKE_FIND_ROOT_PATH})

else()
set(MESON_CROSS_ARGS)
endif(CMAKE_CROSSCOMPILING)

include(cmake/utils.cmake)
include(cmake/wlroots.cmake)

add_subdirectory(3rdparty/glad)
add_subdirectory(3rdparty/sti)
add_subdirectory(3rdparty/glm)

add_subdirectory(libcoreui_wl)
add_subdirectory(flutter_embedder)
add_subdirectory(modules)
add_subdirectory(coreui)
