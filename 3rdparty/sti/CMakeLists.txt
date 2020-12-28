project(sti)

add_library(sti_image STATIC sti_image.c)
target_include_directories(sti_image INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

