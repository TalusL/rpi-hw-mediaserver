cmake_minimum_required(VERSION 3.10)
project(rpi-hw-mediaserver)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(zlmediakit)

set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")

set(RPI_VC_DIR "/opt/vc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi")
link_directories(${RPI_VC_DIR}/lib)

#set header
INCLUDE_DIRECTORIES(zlmediakit/3rdpart/ZLToolKit/src)
INCLUDE_DIRECTORIES(zlmediakit/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/3rdpart)
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/v4l2/inc)

INCLUDE_DIRECTORIES(${RPI_VC_DIR}/include)
INCLUDE_DIRECTORIES(${RPI_VC_DIR}/src/hello_pi/libs/ilclient)

file(GLOB_RECURSE server_src src/*.cpp src/*.c ${RPI_VC_DIR}/src/hello_pi/libs/ilclient/*.c)

add_executable(rpi-hw-mediaserver main.cpp ${server_src} src/VideoEncoder.cpp src/VideoEncoder.h)
target_link_libraries(rpi-hw-mediaserver zlmediakit zltoolkit ssl crypto mpeg mov flv pthread openmaxil bcm_host vcos vchiq_arm rt m)