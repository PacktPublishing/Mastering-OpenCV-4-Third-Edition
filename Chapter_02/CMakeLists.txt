cmake_minimum_required(VERSION 3.12)
project(ch2_sfm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

find_package(Ceres QUIET)

set(OpenCV_DIR "" CACHE PATH "Path to OpenCV build directory")
find_package(OpenCV 4 REQUIRED core calib3d features2d sfm viz)

find_package(Eigen3 REQUIRED)

set(OpenMVS_DIR "" CACHE PATH "Path to OpenMVS build directory")
find_package(OpenMVS REQUIRED)
find_package(CGAL 4 REQUIRED)
find_package(Boost REQUIRED filesystem graph)

include_directories(${EIGEN3_INCLUDE_DIR} ${OpenMVS_INCLUDE_DIRS})

add_executable(ch2_sfm main.cpp)
target_link_libraries(ch2_sfm
        ${OpenCV_LIBRARIES}
        ${Boost_LIBRARIES}
        ${OpenMVS_LIBRARIES})
