cmake_minimum_required (VERSION 3.0) 

PROJECT(ANPR)
SET(PROJECT_VERSION "0.2")

set (CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Requires OpenCV
FIND_PACKAGE( OpenCV 4.0.0 REQUIRED )
MESSAGE("OpenCV version : ${OpenCV_VERSION}")

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

SET(SRC
 main.cpp
 DetectRegions.cpp
 OCR.cpp
 Plate.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME} ${SRC} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}  ${OpenCV_LIBS} )

#ADD_EXECUTABLE( trainSVM trainSVM.cpp )
#TARGET_LINK_LIBRARIES( trainSVM  ${OpenCV_LIBS} )

#ADD_EXECUTABLE( trainOCR trainOCR.cpp OCR.cpp )
#TARGET_LINK_LIBRARIES( trainOCR  ${OpenCV_LIBS} )

#ADD_EXECUTABLE( evalOCR evalOCR.cpp OCR.cpp )
#TARGET_LINK_LIBRARIES( evalOCR  ${OpenCV_LIBS} )

SET(RESOURCES
	README.txt
	SVM.xml
	OCR.xml
	test
)
foreach(f ${RESOURCES})
file( COPY ${f} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
endforeach(f)
