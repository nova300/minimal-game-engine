cmake_minimum_required(VERSION 3.0.0)
project(engine VERSION 0.1.0)

include(CTest)
enable_testing()



add_executable(engine engine.c graphics.c init.c ray.c event.c term.c)

set(LIB m SDL2 SDL2_image SDL2_ttf)
target_link_libraries(engine ${LIB})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
