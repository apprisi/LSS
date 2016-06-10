cmake_minimum_required(VERSION 2.8)
project(Minutiae_Detection)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Minutia_Detection.cpp)
add_executable(Minutiae_Detection ${SOURCE_FILES})
target_link_libraries(Minutiae_Detection ${OpenCV_LIBS} )