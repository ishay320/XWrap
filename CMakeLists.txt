cmake_minimum_required(VERSION 3.10)
project(XWrap-project)

find_package(X11 REQUIRED)

add_executable(main_header src/main.c xwrap.h)
target_link_libraries(main_header PRIVATE X11::X11)
