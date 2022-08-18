project(coreui_wl)

if (NOT UTILS_LOADED)
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
endif()

set(WL_PROTO_HEADER ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.h)
set(WL_PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c)
set(WLR_LAYER_SHELL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/wlr-layer-shell-unstable-v1-protocol.h)
set(WLR_LAYER_SHELL_SRC ${CMAKE_CURRENT_BINARY_DIR}/wlr-layer-shell-unstable-v1-protocol.c)

compile_client_protocol(${CMAKE_SOURCE_DIR}/3rdparty/wayland-protocols/stable/xdg-shell/xdg-shell.xml
        ${WL_PROTO_HEADER} ${WL_PROTO_SRC})
compile_client_protocol(${CMAKE_SOURCE_DIR}/3rdparty/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml
    ${WLR_LAYER_SHELL_HEADER} ${WLR_LAYER_SHELL_SRC})

pkg_check_modules(WL_CURSOR wayland-cursor REQUIRED)
pkg_check_modules(WEGL wayland-egl REQUIRED)
pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(XKB xkbcommon REQUIRED)


set(libcoreui_wl_SRC
    ${WL_PROTO_SRC}
    ${WLR_LAYER_SHELL_SRC}
    application.cpp
    event_loop.cpp
    window.cpp
    wayland/surface.cpp
    wayland/egl_wayland.c
    wayland/registry.c
    wayland/wl_context.c
    wayland/output.c
    wayland/input.c
    wayland/wl_context.cpp
    wayland/layer_shell_surface.c
    wayland/layer_shell_surface.cpp
    wayland/xdg_shell_surface.cpp)

set(INLCUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${XKB_INCLUDE_DIRS}
    ${WL_CURSOR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${INLCUDE_DIRS})

add_library(${PROJECT_NAME} ${libcoreui_wl_SRC})
target_link_libraries(${PROJECT_NAME}
    LINK_INTERFACE_LIBRARIES ${WEGL_LINK_LIBRARIES}
        ${EGL_LINK_LIBRARIES}
        ${WL_CURSOR_LINK_LIBRARIES}
        ${XKB_LINK_LIBRARIES})
target_include_directories(${PROJECT_NAME}
    INTERFACE ${INLCUDE_DIRS})
