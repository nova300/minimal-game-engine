cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0)

include(CTest)
enable_testing()


set(BGFX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include bx/include bimg/include)
include_directories(${BGFX_HEADERS})


set(LIB m SDL2 SDL2_image SDL2_ttf)
set(LIB_BGFX_WIN ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/.build/win64_vs2017/bin/bgfxRelease.lib)
set(LIB_BGFX_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/.build/linux64_gcc/bin/libbgfx-shared-libRelease.so)

add_executable(engine engine.c graphics.c init.c ray.c event.c term.c)
target_link_libraries(engine ${LIB_BGFX_LINUX} ${LIB})

add_executable(engine_win engine.c graphics.c init.c ray.c event.c term.c)
target_link_libraries(engine_win ${LIB_BGFX_WIN} ${LIB})

add_custom_command(TARGET engine_win POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine_win>/media)

add_custom_command(TARGET engine POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:engine>/media)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
