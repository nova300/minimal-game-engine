cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0)

include(CTest)
enable_testing()



add_executable(engine engine.c graphics.c init.c ray.c event.c term.c)
add_executable(engine-win engine.c graphics.c init.c ray.c event.c term.c)

set(LIB m SDL2 GL GLU SDL2_image SDL2_ttf)
target_link_libraries(engine ${LIB})

set(LIBWIN m SDL2 opengl32 glu32 SDL2_image SDL2_ttf)
target_link_libraries(engine-win ${LIBWIN})

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

add_custom_command(TARGET engine-win POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
