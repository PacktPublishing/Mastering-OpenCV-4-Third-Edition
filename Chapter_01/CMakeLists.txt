cmake_minimum_required (VERSION 3.0) 

PROJECT(Cartoonifier)

set (CMAKE_CXX_STANDARD 11)

# Requires OpenCV
FIND_PACKAGE( OpenCV 4.0.0 REQUIRED )

SET(SRC
    main.cpp
    cartoon.cpp
   # ImageUtils_0.7.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME} ${SRC} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}  ${OpenCV_LIBS} )
