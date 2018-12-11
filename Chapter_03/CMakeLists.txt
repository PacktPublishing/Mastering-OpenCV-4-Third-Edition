cmake_minimum_required(VERSION 3.12)
project(ch3_face)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "" CACHE PATH "OpenCV 4 build Directory")
find_package(OpenCV 4 REQUIRED core face highgui calib3d)

find_package(Boost REQUIRED system)

include_directories(${Boost_INCLUDE_DIR})

add_executable(ch3_face main.cpp)

target_link_libraries(ch3_face ${OpenCV_LIBRARIES})
