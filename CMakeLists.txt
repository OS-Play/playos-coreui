cmake_minimum_required(VERSION 3.10.2)

project(coreui)

option(BUILD_WLROOTS "Build wlroots" ON)
option(EP_UPDATE_DISCONNECTED "ExternalProject update disconnected" OFF)

if (${EP_UPDATE_DISCONNECTED})
  set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY EP_UPDATE_DISCONNECTED TRUE)
endif()

set(WAYLAND_PROTOCOLS ${CMAKE_SOURCE_DIR}/3rdparty/wayland-protocols)
add_definitions(-DWLR_USE_UNSTABLE -Werror)

find_package(PkgConfig)

pkg_check_modules(PIXMAN pixman-1 REQUIRED)
include_directories(${PIXMAN_INCLUDE_DIRS})

include(cmake/coreui-utils.cmake)
include(cmake/wlroots.cmake)
include(cmake/iniparser.cmake)
include(cmake/rapidjson.cmake)

add_subdirectory(3rdparty/glad)
add_subdirectory(3rdparty/sti)
add_subdirectory(3rdparty/glm)

add_subdirectory(libcoreui_wl)
add_subdirectory(libcoreui_flutter)
add_subdirectory(modules)
add_subdirectory(coreui)
