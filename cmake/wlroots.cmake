set(WLR_BINARY_DIR ${CMAKE_BINARY_DIR}/3rdparty/wlroots)
set(WLR_LIB wlroots)
set(WLR_LIB_STATIC wlroots_static)

find_program(MESON meson REQUIRED)
find_program(NINJA ninja REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(DRM libdrm REQUIRED)
pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(GLES2 glesv2 REQUIRED)
pkg_check_modules(UDEV libudev REQUIRED)
pkg_check_modules(PIXMAN pixman-1 REQUIRED)
pkg_check_modules(WS wayland-server REQUIRED)
pkg_check_modules(WC wayland-client REQUIRED)
pkg_check_modules(GBM gbm REQUIRED)
pkg_check_modules(SEAT libseat REQUIRED)
pkg_check_modules(XKB xkbcommon REQUIRED)
pkg_check_modules(XCB xcb REQUIRED)
pkg_check_modules(XCB_RU xcb-renderutil REQUIRED)
pkg_check_modules(XCB_ERROR xcb-errors REQUIRED)

set(WLR_DEPENDENCIES "${EGL_LINK_LIBRARIES} ${DRM_LINK_LIBRARIES} ${GLES2_LINK_LIBRARIES} -lm")
set(WLR_DEPENDENCIES_DIRS "${EGL_LIBRARY_DIRS} ${DRM_LIBRARY_DIRS} ${GLES2_LIBRARY_DIRS}")

include(ExternalProject)
ExternalProject_Add(${WLR_LIB}_build
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/wlroots
    CONFIGURE_COMMAND ${MESON} <BINARY_DIR> <SOURCE_DIR> --default-library=both --prefix=${WLR_BINARY_DIR}
    BUILD_COMMAND ${NINJA} -C <BINARY_DIR>
    INSTALL_COMMAND ${NINJA} -C <BINARY_DIR> install)

add_library(${WLR_LIB_STATIC} STATIC IMPORTED)
add_dependencies(${WLR_LIB_STATIC} ${WLR_LIB}_build)
set_target_properties(${WLR_LIB_STATIC} PROPERTIES
    IMPORTED_LOCATION ${WLR_BINARY_DIR}/lib/libwlroots.a
    INTERFACE_INCLUDE_DIRECTORIES ${WLR_BINARY_DIR}/include
    # INTERFACE_LINK_LIBRARIES ${WLR_DEPENDENCIES}
    INTERFACE_LINK_DIRECTORIES ${WLR_DEPENDENCIES_DIRS})
set_property(TARGET ${WLR_LIB_STATIC}
    APPEND
    PROPERTY INTERFACE_LINK_LIBRARIES
    ${EGL_LINK_LIBRARIES}
    ${DRM_LINK_LIBRARIES}
    ${GLES2_LINK_LIBRARIES}
    ${UDEV_LINK_LIBRARIES}
    ${PIXMAN_LINK_LIBRARIES}
    ${WS_LINK_LIBRARIES}
    ${WC_LINK_LIBRARIES}
    ${GBM_LINK_LIBRARIES}
    ${SEAT_LINK_LIBRARIES}
    ${XKB_LINK_LIBRARIES}
    ${XCB_LINK_LIBRARIES}
    ${XCB_RU_LINK_LIBRARIES}
    ${XCB_ERROR_LINK_LIBRARIES}
    -lm)

add_library(${WLR_LIB} SHARED IMPORTED)
add_dependencies(${WLR_LIB} ${WLR_LIB}_build)
set_target_properties(${WLR_LIB} PROPERTIES IMPORTED_LOCATION
    ${WLR_BINARY_DIR}/lib/libwlroots.so
    INTERFACE_INCLUDE_DIRECTORIES ${WLR_BINARY_DIR}/include)
include_directories(${WLR_BINARY_DIR}/include)
