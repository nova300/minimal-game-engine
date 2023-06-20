cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0 LANGUAGES C)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_UNITY_BUILD TRUE)
SET(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)

add_executable(engine lib/glew.c src/engine.c src/graphics.c src/shaders.c src/geometry.c src/ray.c src/event.c src/shapes.c src/2d.c src/init.c)
set(LIB m GL)
target_link_libraries(engine ${LIB} glfw )


add_executable(engine-win lib/glew.c src/engine.c src/graphics.c src/shaders.c src/geometry.c src/ray.c src/event.c src/shapes.c src/2d.c src/init.c)
set(winlib m opengl32)
target_link_libraries(engine-win ${winlib} glfw)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

add_custom_command(TARGET engine-win POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine-win>/media)

#include(./Harden.cmake)