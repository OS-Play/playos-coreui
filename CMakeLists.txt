cmake_minimum_required(VERSION 3.10.2)

project(coreui)

set(WAYLAND_PROTOCOLS ${CMAKE_SOURCE_DIR}/3rdparty/wayland-protocols)
add_definitions(-DWLR_USE_UNSTABLE -Werror)

find_package(PkgConfig)

include(cmake/utils.cmake)
include(cmake/wlroots.cmake)

add_subdirectory(3rdparty/glad)
add_subdirectory(3rdparty/sti)
add_subdirectory(3rdparty/glm)

add_subdirectory(libcoreui_wl)
add_subdirectory(flutter_embedder)
add_subdirectory(modules)
add_subdirectory(coreui)
