cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0)

include(CTest)
enable_testing()



add_executable(engine src/engine.c src/graphics.c src/shaders.c src/2d.c src/geometry.c src/init.c src/ray.c src/event.c src/term.c)
set(LIB m SDL2 GLEW GL GLU SDL2_image SDL2_ttf)
target_link_libraries(engine ${LIB})


add_executable(engine-win lib/glew.c src/engine.c src/graphics.c src/shaders.c src/2d.c src/geometry.c src/init.c src/ray.c src/event.c src/term.c)
set(winlib m SDL2 opengl32 glu32 SDL2_image SDL2_ttf)
target_link_libraries(engine-win ${winlib})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

add_custom_command(TARGET engine-win POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine-win>/media)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
