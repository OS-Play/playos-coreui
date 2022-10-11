project(playos-wallpaper VERSION 0.0.1)

set(WP_SRC main.cpp
    application.cpp
    wallpaper.cpp)

add_wl_executable(${PROJECT_NAME} ${WP_SRC})
target_link_libraries(${PROJECT_NAME} coreui_wl glad sti_image glm::glm)
