cmake_minimum_required(VERSION 3.0)

project(WebcamFaceRec)

set(CMAKE_CXX_STANDARD 11)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

# Requires OpenCV v4.0 or later
find_package(OpenCV 4 REQUIRED COMPONENTS core highgui dnn face)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

set(SRC main.cpp detectObject.cpp preprocessFace.cpp recognition.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} ${OpenCV_LIBS})

add_executable(dnn_face_detector resnet_ssd_face.cpp)
target_link_libraries(dnn_face_detector ${CONAN_LIBS} ${OpenCV_LIBS})
