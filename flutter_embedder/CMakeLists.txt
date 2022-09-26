project(flutter_embedder)

add_definitions(-DRAPIDJSON_HAS_STDSTRING)

set(FE_SRC
    application.cpp
    coreui_embedder.cpp
    coreui_compositor.cpp
    plugins/coreui_key_event.cpp
    plugins/coreui_text_input.cpp
    internal/json_message_codec.cpp
    internal/json_method_codec.cpp)
    # internal/coreui_channel.cpp
    # internal/coreui_messenger.cpp
    # internal/coreui_plugin_registrar.cpp)

if (NOT ENV{FLUTTER_ENGINE_SOURCE_DIR})
    set(FLUTTER_ENGINE_SOURCE_DIR /prodata/flutter/engine/src)
else()
    set(FLUTTER_ENGINE_SOURCE_DIR $ENV{FLUTTER_ENGINE_SOURCE_DIR})
endif()

if (NOT FLUTTER_SOURCE_DIR)
set(FLUTTER_SOURCE_DIR ${FLUTTER_ENGINE_SOURCE_DIR}/flutter)
endif()

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FLUTTER_ENGINE_SOURCE_DIR}
    ${FLUTTER_SOURCE_DIR}/shell/platform/common/client_wrapper/include
    ${FLUTTER_SOURCE_DIR}/shell/platform/common/public
    ${FLUTTER_SOURCE_DIR}/shell/platform/embedder
    ${FLUTTER_ENGINE_SOURCE_DIR}/third_party/rapidjson/include/)

include_directories(${INCLUDE_DIRS})

if (CMAKE_CROSSCOMPILING)
list(APPEND CMAKE_FIND_ROOT_PATH $ENV{FLUTTER_OUTPUT_DIR})
endif()

find_library(FLUTTER_LIB flutter_engine
    NAMES libflutter_engine.so
    PATHS ENV FLUTTER_OUTPUT_DIR
        ${FLUTTER_ENGINE_SOURCE_DIR}/out/host_debug_unopt
        ${FLUTTER_ENGINE_SOURCE_DIR}/out/host_unopt
    REGISTRY_VIEW TARGET
    REQUIRED)

set(FLUTTER_ENGINE_SRC
    ${FLUTTER_SOURCE_DIR}/shell/platform/common/client_wrapper/core_implementations.cc
    ${FLUTTER_SOURCE_DIR}/shell/platform/common/incoming_message_dispatcher.cc
    ${FLUTTER_SOURCE_DIR}/shell/platform/common/client_wrapper/plugin_registrar.cc)


add_library(${PROJECT_NAME} STATIC ${FE_SRC} ${FLUTTER_ENGINE_SRC})
target_link_libraries(${PROJECT_NAME} ${FLUTTER_LIB} coreui_wl glad glm::glm)
target_include_directories(${PROJECT_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
    INTERFACE ${INCLUDE_DIRS})
