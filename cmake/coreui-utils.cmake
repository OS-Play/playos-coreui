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

macro(add_wl_executable appname ...)
set("${appname}_SRC" ${ARGV})
list(REMOVE_ITEM "${appname}_SRC" ${appname})
add_executable(${appname} ${${appname}_SRC})

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/meta.xml FILE_CONTENTS)
string(REPLACE "@APP_VERSION@" "${PROJECT_VERSION}" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/meta.xml "${FILE_CONTENTS}")

set(${appname}_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/Applications/${appname}.app)
install(CODE "execute_process(COMMAND rm ${${appname}_INSTALL_PATH}/ -rf)")
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${appname}
        DESTINATION ${${appname}_INSTALL_PATH}/bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources OPTIONAL
        DESTINATION ${${appname}_INSTALL_PATH}/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/meta.xml DESTINATION ${${appname}_INSTALL_PATH}/)
endmacro()

macro(add_flutter_executable appname flutterPath ...)
find_program(FLUTTER flutter)

# Generate all dart relative files
file(GLOB_RECURSE ${appname}_dart_src
        LIST_DIRECTORIES TRUE
        FOLLOW_SYMLINKS
        ${flutterPath}/lib/*.dart)

if (NOT CMAKE_BUILD_TYPE MATCHES "debug|Debug|DEBUG")
set(${appname}_BUILD_TYPE --release)
set(PLAYOS_FLUTTER_SDK_PATH $ENV{PLAYOS_FLUTTER_SDK_PATH})

if (NOT PLAYOS_FLUTTER_SDK_PATH)
message(FATAL_ERROR "No PLAYOS_FLUTTER_SDK_PATH environment variable found.")
endif()

find_program(DART dart REQUIRED)
find_program(GEN_SNAPSHOT gen_snapshot REQUIRED)

add_custom_command(OUTPUT "${appname}_kernel_snapshot.dill"
        WORKING_DIRECTORY ${flutterPath}
        COMMAND ${DART}
            ${PLAYOS_FLUTTER_SDK_PATH}/frontend_server.dart.snapshot
            --sdk-root ${PLAYOS_FLUTTER_SDK_PATH}/flutter_patched_sdk/
            --target=flutter
            --aot
            --tfa
            -Ddart.vm.product=true
            --packages .dart_tool/package_config.json
            --output-dill ${CMAKE_CURRENT_BINARY_DIR}/${appname}_kernel_snapshot.dill
            --verbose
            --depfile ${CMAKE_CURRENT_BINARY_DIR}/${appname}_kernel_snapshot.d
            lib/main.dart
        DEPENDS ${${appname}_dart_src})
add_custom_target("${appname}_app.so"
        WORKING_DIRECTORY ${flutterPath}
        COMMAND ${GEN_SNAPSHOT}
            --snapshot_kind=app-aot-elf
            --elf=${CMAKE_CURRENT_BINARY_DIR}/${appname}_app.so
            ${CMAKE_CURRENT_BINARY_DIR}/${appname}_kernel_snapshot.dill
        DEPENDS ${appname}_kernel_snapshot.dill)
else()
set(${appname}_BUILD_TYPE "")
endif()

add_custom_target("${appname}_fl_build"
        WORKING_DIRECTORY ${flutterPath}
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/resources/flutter_assets
        COMMAND ${FLUTTER} build bundle ${${appname}_BUILD_TYPE} --asset-dir ${CMAKE_CURRENT_BINARY_DIR}/resources/flutter_assets)

set("${appname}_FL_SRC" ${ARGV})
list(REMOVE_ITEM "${appname}_FL_SRC" ${appname} ${flutterPath})
add_wl_executable(${appname} ${${appname}_FL_SRC})
add_dependencies(${appname} "${appname}_fl_build")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources OPTIONAL
        DESTINATION ${${appname}_INSTALL_PATH}/)
if (NOT CMAKE_BUILD_TYPE MATCHES "debug|Debug|DEBUG")
    add_dependencies("${appname}_app.so" "${appname}_fl_build")
    add_dependencies(${appname} "${appname}_app.so")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${appname}_app.so OPTIONAL
            DESTINATION ${${appname}_INSTALL_PATH}/bin
            RENAME app.so)
endif()
endmacro()
