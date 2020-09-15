cmake_minimum_required(VERSION 3.0)

project(ANPR)
set(PROJECT_VERSION "0.2")

set(CMAKE_CXX_STANDARD 11)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

# Requires OpenCV
find_package(OpenCV 4.0.0 REQUIRED COMPONENTS core highgui ml flann dnn)
message(STATUS "OpenCV version : ${OpenCV_VERSION}")

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

set(SRC main.cpp DetectRegions.cpp OCR.cpp Plate.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

add_executable(trainSVM trainSVM.cpp)
target_link_libraries(trainSVM ${OpenCV_LIBS})

add_executable(trainOCR trainOCR.cpp OCR.cpp)
target_link_libraries(trainOCR ${OpenCV_LIBS})

add_executable(evalOCR evalOCR.cpp OCR.cpp)
target_link_libraries(evalOCR ${OpenCV_LIBS})

set(RESOURCES README.md SVM.xml OCR.xml test)
foreach(f ${RESOURCES})
  file(COPY ${f} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
endforeach(f)
