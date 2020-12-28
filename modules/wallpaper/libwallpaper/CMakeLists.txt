project(wallpaper)

find_package(PkgConfig REQUIRED)

if (NOT WLR_LIB)
include(${CMAKE_SOURCE_DIR}/cmake/wlroots.cmake)
endif()

if (NOT UTILS_LOADED)
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
endif()

pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(WC wayland-client REQUIRED)
pkg_check_modules(WL_EGL wayland-egl REQUIRED)
pkg_check_modules(GLESv2 glesv2 REQUIRED)


set(WL_PROTO_HEADER ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.h)
set(WL_PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c)
set(WLR_LAYER_SHELL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/wlr-layer-shell-unstable-v1-protocol.h)
set(WLR_LAYER_SHELL_SRC ${CMAKE_CURRENT_BINARY_DIR}/wlr-layer-shell-unstable-v1-protocol.c)

compile_client_protocol(${WAYLAND_PROTOCOLS}/stable/xdg-shell/xdg-shell.xml ${WL_PROTO_HEADER} ${WL_PROTO_SRC})
compile_client_protocol(${CMAKE_SOURCE_DIR}/3rdparty/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml ${WLR_LAYER_SHELL_HEADER} ${WLR_LAYER_SHELL_SRC})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${EGL_INCLUDE_DIRS})

set(WP_SRC wallpaper.c
    output.c
    gles.cpp
    egl_common.c
    ${WL_PROTO_SRC}
    ${WLR_LAYER_SHELL_SRC})

set(WP_LINK glad glm::glm ${EGL_LINK_LIBRARIES} ${WL_EGL_LINK_LIBRARIES} ${GLESv2_LINK_LIBRARIES})

add_library(${PROJECT_NAME} STATIC ${WP_SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC ${WP_LINK})
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_LINK_LIBRARIES ${WLR_LIB})
set_property(TARGET ${PROJECT_NAME}
    APPEND
    PROPERTY
    INTERFACE_LINK_LIBRARIES ${WC_LINK_LIBRARIES} ${WP_LINK})
