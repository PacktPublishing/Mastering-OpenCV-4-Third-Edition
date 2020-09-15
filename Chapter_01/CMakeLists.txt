cmake_minimum_required(VERSION 3.0)

project(Cartoonifier)

set(CMAKE_CXX_STANDARD 11)

# Requires OpenCV
find_package(OpenCV 4.0.0 REQUIRED)

set(SRC main.cpp cartoon.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
