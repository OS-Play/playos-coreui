project(playos-wallpaper)

set(WP_SRC main.cpp
    application.cpp
    wallpaper.cpp)

add_executable(${PROJECT_NAME} ${WP_SRC})
target_link_libraries(${PROJECT_NAME} EGL coreui_wl glad sti_image)
