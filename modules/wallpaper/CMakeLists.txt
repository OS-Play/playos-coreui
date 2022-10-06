project(playos-wallpaper)

set(WP_SRC main.cpp
    application.cpp
    wallpaper.cpp)

add_executable(${PROJECT_NAME} ${WP_SRC})
target_link_libraries(${PROJECT_NAME} coreui_wl glad sti_image glm::glm)

install(TARGETS ${PROJECT_NAME})
