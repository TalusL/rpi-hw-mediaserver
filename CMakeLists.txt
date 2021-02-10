cmake_minimum_required(VERSION 3.10)
project(rpi-hw-mediaserver)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(zlmediakit)

set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")


#set header
INCLUDE_DIRECTORIES(zlmediakit/3rdpart/ZLToolKit/src)
INCLUDE_DIRECTORIES(zlmediakit/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/3rdpart)
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/v4l2/inc)

file(GLOB_RECURSE server_src src/*.cpp src/*.c)

add_executable(rpi-hw-mediaserver main.cpp ${server_src})
target_link_libraries(rpi-hw-mediaserver zlmediakit zltoolkit ssl crypto mpeg mov flv pthread)