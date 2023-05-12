cmake_minimum_required(VERSION 3.10)
project(XWrap-project)

find_package(X11 REQUIRED)

include_directories(include)

add_library(xwrap src/xwrap.c include/xwrap.h)
target_link_libraries(xwrap PUBLIC X11::X11)

add_executable(main src/main.c)
target_link_libraries(main PRIVATE xwrap)
