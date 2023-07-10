cmake_minimum_required(VERSION 3.20.0)

project(gotfreed VERSION 1.0)

add_library(got got/got.c got/got.h)
add_executable(gotfreed gotfreed.c got/got.h)

include_directories("${CMAKE_SOURCE_DIR}/got")

target_link_libraries(gotfreed got)
