set(APP_NAME Wallpaper)

add_subdirectory(libwallpaper)

add_executable(${APP_NAME} main.cpp)
target_link_libraries(${APP_NAME} wallpaper sti_image)
