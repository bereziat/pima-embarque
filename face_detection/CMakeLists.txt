cmake_minimum_required(VERSION 2.8)
project(camcv)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-g -Wall -Werror -O3 -std=c++11")
set(CMAKE_C_FLAGS "-g -Werror -O3")

find_package(OpenCV REQUIRED)

include_directories(userland/host_applications/linux/libs/bcm_host/include)
include_directories(userland/interface/vcos)
include_directories(userland)
include_directories(userland/interface/vcos/pthreads)
include_directories(userland/interface/vmcs_host/linux)
include_directories(userland/interface/khronos/include)
include_directories(.)

set (GL_SCENE_SOURCES
   gl_scenes/models.c
   gl_scenes/mirror.c
   gl_scenes/yuv.c
   gl_scenes/sobel.c
   gl_scenes/square.c
   gl_scenes/teapot.c)

set (COMMON_SOURCES
   RaspiCamControl.c
   RaspiCLI.c
   RaspiPreview.c)

add_executable(camcv ${COMMON_SOURCES} camcv.c facedetect.cpp RaspiTex.c RaspiTexUtil.c tga.c ${GL_SCENE_SOURCES})

target_link_libraries(camcv
  userland/build/lib/libmmal_core.so
  userland/build/lib/libmmal_util.so
  userland/build/lib/libmmal_vc_client.so
  userland/build/lib/libvcos.so
  userland/build/lib/libbcm_host.so
  userland/build/lib/libGLESv2.so
  userland/build/lib/libEGL.so
  pthread
  m
  ${OpenCV_LIBS})
