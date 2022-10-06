project(playos-panel)

find_package(PkgConfig)
pkg_check_modules(WC wayland-client REQUIRED)
pkg_check_modules(WEGL wayland-egl REQUIRED)
pkg_check_modules(WCC wayland-cursor REQUIRED)

set(PANEL_SRC main.cpp
    application.cpp
    ${WL_PROTO_SRC}
    ${WLR_LAYER_SHELL_SRC})

include_directories(
    ${WC_INCLUDE_DIRS}
    ${WEGL_INCLUDE_DIRS}
    ${WCC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})

add_flutter_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/gui ${PANEL_SRC})
target_link_libraries(${PROJECT_NAME} ${WC_LIBRARIES}
     ${WEGL_LIBRARIES} ${WLR_LIB} EGL GLESv2 ${WCC_LIBRARIES} flutter_embedder)

install(TARGETS ${PROJECT_NAME})
