set(INI_PARSER_BINARY_DIR ${CMAKE_BINARY_DIR}/3rdparty/iniparser)
set(INI_PARSER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/iniparser)

include(ExternalProject)

find_program(MAKE make REQUIRED)

ExternalProject_Add(iniparser_build
    GIT_REPOSITORY https://github.com/ndevilla/iniparser.git
    GIT_TAG v4.1
    SOURCE_DIR ${INI_PARSER_SOURCE_DIR}
    BINARY_DIR ${INI_PARSER_BINARY_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MAKE} -C <SOURCE_DIR>
    INSTALL_COMMAND sh -c "cp <SOURCE_DIR>/src/*.h <SOURCE_DIR>/*.a <BINARY_DIR>")


add_library(iniparser STATIC IMPORTED)
add_dependencies(iniparser iniparser_build)
set_target_properties(iniparser PROPERTIES
    IMPORTED_LOCATION ${INI_PARSER_BINARY_DIR}/libiniparser.a)
include_directories(${INI_PARSER_BINARY_DIR})
