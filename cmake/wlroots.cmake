set(WLR_BINARY_DIR ${CMAKE_BINARY_DIR}/3rdparty/wlroots)
set(WLR_LIB wlroots)
set(WLR_LIB_STATIC wlroots_static)

find_package(PkgConfig REQUIRED)

pkg_check_modules(DRM libdrm REQUIRED)
pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(GLES2 glesv2 REQUIRED)
pkg_check_modules(UDEV libudev REQUIRED)
pkg_check_modules(PIXMAN pixman-1 REQUIRED)
pkg_check_modules(WS wayland-server IMPORTED_TARGET REQUIRED)
pkg_check_modules(WC wayland-client REQUIRED)
pkg_check_modules(GBM gbm REQUIRED)
pkg_check_modules(SEAT libseat REQUIRED)
pkg_check_modules(XKB xkbcommon IMPORTED_TARGET REQUIRED)
# pkg_check_modules(XCB xcb REQUIRED)
# pkg_check_modules(XCB_RU xcb-renderutil REQUIRED)
# pkg_check_modules(XCB_ERROR xcb-errors REQUIRED)

set(WLR_LINK_EXTRA_LIBRARIES ${EGL_LIBRARIES}
        ${DRM_LIBRARIES}
        ${GLES2_LIBRARIES}
        ${UDEV_LIBRARIES}
        ${PIXMAN_LIBRARIES}
        PkgConfig::WS
        ${WC_LIBRARIES}
        ${GBM_LIBRARIES}
        ${SEAT_LIBRARIES}
        ${XKB_LIBRARIES}
        ${XCB_LIBRARIES}
        ${XCB_RU_LIBRARIES}
        ${XCB_ERROR_LIBRARIES}
        -lm)

set(WLR_DEPENDENCIES "${EGL_LIBRARIES} ${DRM_LIBRARIES} ${GLES2_LIBRARIES} -lm")
set(WLR_DEPENDENCIES_DIRS "${EGL_LIBRARY_DIRS} ${DRM_LIBRARY_DIRS} ${GLES2_LIBRARY_DIRS}")

if (BUILD_WLROOTS)

find_program(MESON meson REQUIRED)
find_program(NINJA ninja REQUIRED)

include(ExternalProject)
ExternalProject_Add(${WLR_LIB}_build
    GIT_REPOSITORY https://gitlab.freedesktop.org/wlroots/wlroots.git
    GIT_TAG 30bf8a4303bc5df3cb87b7e6555592dbf8d95cf1
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/wlroots
    UPDATE_DISCONNECTED ${FETCHCONTENT_UPDATES_DISCONNECTED}
    CONFIGURE_COMMAND ${MESON} --debug --default-library=both --prefix=${WLR_BINARY_DIR} ${MESON_CROSS_ARGS} <BINARY_DIR> <SOURCE_DIR>
    BUILD_COMMAND ${NINJA} -C <BINARY_DIR>
    INSTALL_COMMAND ${NINJA} -C <BINARY_DIR> install)

# Pre create include dirs
exec_program(mkdir ARGS -p ${WLR_BINARY_DIR}/include)

add_library(${WLR_LIB_STATIC} STATIC IMPORTED)
add_dependencies(${WLR_LIB_STATIC} ${WLR_LIB}_build)
set_target_properties(${WLR_LIB_STATIC} PROPERTIES
    IMPORTED_LOCATION ${WLR_BINARY_DIR}/lib/libwlroots.a
    INTERFACE_INCLUDE_DIRECTORIES ${WLR_BINARY_DIR}/include
    INTERFACE_LINK_DIRECTORIES ${WLR_DEPENDENCIES_DIRS})
set_property(TARGET ${WLR_LIB_STATIC}
    APPEND
    PROPERTY INTERFACE_LINK_LIBRARIES ${WLR_LINK_EXTRA_LIBRARIES})

add_library(${WLR_LIB} SHARED IMPORTED)
add_dependencies(${WLR_LIB} ${WLR_LIB}_build)
set_target_properties(${WLR_LIB} PROPERTIES IMPORTED_LOCATION
    ${WLR_BINARY_DIR}/lib/libwlroots.so
    INTERFACE_INCLUDE_DIRECTORIES ${WLR_BINARY_DIR}/include)
include_directories(${WLR_BINARY_DIR}/include)
set_property(TARGET ${WLR_LIB}
    APPEND
    PROPERTY INTERFACE_LINK_LIBRARIES ${WLR_LINK_EXTRA_LIBRARIES})

else(BUILD_WLROOTS)

pkg_check_modules(WLR wlroots REQUIRED)
include_directories(${WLR_INCLUDE_DIRS})
set(WLR_LIB ${WLR_LINK_LIBRARIES} ${WLR_LINK_EXTRA_LIBRARIES})

endif(BUILD_WLROOTS)
