cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0)

add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)



add_executable(engine src/engine.c src/graphics.c src/shaders.c src/geometry.c src/ray.c src/event.c src/shapes.c src/init.c)
set(LIB m GL GLEW)
target_link_libraries(engine glfw ${OPENGL_glu_LIBRARY} ${GLFW_LIBRARIES} ${LIB})


add_executable(engine-win lib/glew.c src/engine.c src/graphics.c src/shaders.c src/geometry.c src/ray.c src/event.c src/shapes.c src/init.c)
set(winlib m opengl32)
target_link_libraries(engine-win glfw ${OPENGL_glu_LIBRARY} ${GLFW_LIBRARIES} ${winlib})

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
