cmake_minimum_required(VERSION 3.6)
project(webserver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package (Threads)
find_package (OpenSSL)

set(SOURCE_FILES main.cpp QueryHandler.cpp QueryHandler.h Crypto.cpp Crypto.h lib.h)
add_executable(webserver ${SOURCE_FILES})
target_link_libraries (webserver ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (webserver ${OPENSSL_LIBRARIES})

if(APPLE)
    message(STATUS ">>> Apple")
    include_directories(/usr/local/opt/openssl/include/)
endif()
