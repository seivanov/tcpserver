cmake_minimum_required(VERSION 3.6)
project(webserver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package (Threads)

set(SOURCE_FILES main.cpp QueryHandler.cpp QueryHandler.h)
add_executable(webserver ${SOURCE_FILES})
target_link_libraries (webserver ${CMAKE_THREAD_LIBS_INIT})