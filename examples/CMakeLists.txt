cmake_minimum_required(VERSION 3.10)
project(XWrap-examples)

find_package(X11 REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

add_executable(simple simple.c ../xwrap.h)
add_executable(pong pong.c ../xwrap.h)
add_executable(multiwindow multiwindow.c ../xwrap.h)
add_executable(sliders sliders.c ../xwrap.h)

# target_link_libraries(simple PRIVATE X11::X11)
