project(flutter_embedder)

set(FE_SRC
    application.cpp
    embedder.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # $ENV{FLUTTER_SOURCE_DIR}/shell/platform/embedder
    /prodata/flutter/engine/src/flutter/shell/platform/embedder)

find_library(FLUTTER_LIB flutter_engine
    PATHS $ENV{FLUTTER_OUTPUT_DIR} /prodata/flutter/engine/src/out/host_debug_unopt)

add_library(${PROJECT_NAME} STATIC ${FE_SRC})
target_link_libraries(${PROJECT_NAME} ${FLUTTER_LIB} coreui_wl)
target_include_directories(${PROJECT_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
