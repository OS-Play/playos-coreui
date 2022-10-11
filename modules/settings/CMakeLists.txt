project(playos-settings VERSION 0.0.1)

set(SETTINGS_SRC
    main.cpp)

add_flutter_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/gui ${SETTINGS_SRC})
target_link_libraries(${PROJECT_NAME} EGL coreui_flutter)
