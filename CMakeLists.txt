cmake_minimum_required(VERSION 3.10)
project(XWrap-project)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_libraries(${X11_LIBRARIES})

add_executable(xwrap src/main.c)
