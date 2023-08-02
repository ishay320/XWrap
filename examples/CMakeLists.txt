cmake_minimum_required(VERSION 3.10)
project(XWrap-examples)

find_package(X11 REQUIRED)

add_executable(simple simple.c ../xwrap.h)

# target_link_libraries(simple PRIVATE X11::X11)
