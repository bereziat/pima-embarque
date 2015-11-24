cmake_minimum_required (VERSION 2.8)
project (DetectionBalise)

find_package( OpenCV REQUIRED )

add_executable(detection ../main.cpp ../Parameters.cpp ../Processing.cpp ../Face.hpp)
target_link_libraries( detection ${OpenCV_LIBS})
