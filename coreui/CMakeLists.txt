set(WL_PROTO_HEADER ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.h)
set(WL_PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c)
set(WLR_LAYER_SHELL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/wlr-layer-shell-unstable-v1-protocol.h)
set(WLR_LAYER_SHELL_SRC ${CMAKE_CURRENT_BINARY_DIR}/wlr-layer-shell-unstable-v1-protocol.c)
set(COREUI_SRC 
    main.c 
    server.c
    output.c
    views.c
    input.c
    shell/layer-shell.c
    ${WL_PROTO_SRC}
    ${WLR_LAYER_SHELL_SRC})

compile_protocol(${WAYLAND_PROTOCOLS}/stable/xdg-shell/xdg-shell.xml ${WL_PROTO_HEADER} ${WL_PROTO_SRC})
compile_protocol( ${CMAKE_SOURCE_DIR}/3rdparty/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml ${WLR_LAYER_SHELL_HEADER} ${WLR_LAYER_SHELL_SRC})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WLROOTS_INCLUDE_DIRS}
    ${WS_INCLUDE_DIRS}
    ${XKB_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${COREUI_SRC})
target_link_libraries(${PROJECT_NAME} ${WLR_LIB} ${WS_LIBRARIES} ${XKB_LIBRARIES})
