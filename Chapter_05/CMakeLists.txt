cmake_minimum_required (VERSION 3.0) 

PROJECT(WebcamFaceRec)

set (CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Requires OpenCV v4.0 or later
FIND_PACKAGE( OpenCV 4 REQUIRED )

SET(SRC
    main.cpp
    detectObject.cpp
    preprocessFace.cpp
    recognition.cpp
    #ImageUtils_0.7.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME} ${SRC} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}  ${CONAN_LIBS} )

ADD_EXECUTABLE( dnn_face_detector resnet_ssd_face.cpp )
TARGET_LINK_LIBRARIES( dnn_face_detector  ${CONAN_LIBS} )