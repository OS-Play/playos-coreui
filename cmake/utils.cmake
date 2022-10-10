set(UTILS_LOADED true)

macro(init_tools)
if(NOT PKG_CONFIG_FOUND)
find_package(PkgConfig REQUIRED)
endif()

if(NOT WAYLAND_SCANNER OR NOT WAYLAND_PROTOCOLS)
pkg_check_modules(WS wayland-server REQUIRED)

pkg_get_variable(WAYLAND_PROTOCOLS wayland-protocols pkgdatadir)

if (CMAKE_CROSSCOMPILING)
find_program(WAYLAND_SCANNER wayland-scanner REGISTRY_VIEW HOST)
else()
pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)
endif()

endif()
endmacro()

macro(compile_protocol src dest_header dest_source)
init_tools()
add_custom_command(OUTPUT ${dest_header} ${dest_source}
    COMMAND ${WAYLAND_SCANNER} server-header
        ${src} ${dest_header}
    COMMAND ${WAYLAND_SCANNER} private-code
        ${src} ${dest_source}
    DEPENDS ${src})
endmacro()

macro(compile_client_protocol src dest_header dest_source)
init_tools()
add_custom_command(OUTPUT ${dest_header} ${dest_source}
    COMMAND ${WAYLAND_SCANNER} client-header
        ${src} ${dest_header}
    COMMAND ${WAYLAND_SCANNER} private-code
        ${src} ${dest_source}
    DEPENDS ${src})
endmacro()

macro(add_flutter_executable appname flutterPath ...)
find_program(FLUTTER flutter)

add_custom_target("${appname}_fl_build"
    WORKING_DIRECTORY ${flutterPath}
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/resources/flutter_assets
    COMMAND ${FLUTTER} build bundle --asset-dir ${CMAKE_CURRENT_BINARY_DIR}/resources/flutter_assets)

set("${appname}_SRC" ${ARGV})
list(REMOVE_ITEM "${appname}_SRC" ${appname} ${flutterPath})
add_executable(${appname} ${${appname}_SRC})
add_dependencies(${appname} "${appname}_fl_build")

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/meta.xml FILE_CONTENTS)
string(REPLACE "@APP_VERSION@" "${PROJECT_VERSION}" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/meta.xml "${FILE_CONTENTS}")

if (${CMAKE_INSTALL_PREFIX} MATCHES "/usr/local")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/)
endif()

install(CODE "execute_process(COMMAND rm ${CMAKE_INSTALL_PREFIX}/${appname}.app -rf)")
install(TARGETS ${appname} DESTINATION ${appname}.app/bin)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources DESTINATION ${appname}.app)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/meta.xml DESTINATION ${appname}.app)
endmacro()
