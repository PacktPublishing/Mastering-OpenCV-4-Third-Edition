# Chapter 5: Face Recognition using Eigenfaces or Fisherfaces

Note: You need OpenCV v4 or later, otherwise the FaceRecognizer will not compile or run.
And you need atleast 3 Face & Eye detection XML files from OpenCV, as shown below.

## Requirements

- OpenCV 4+ (with components `core, highgui, face, dnn`)
- Optional: Use Conan (https://conan.io/) to install OpenCV (e.g. `[build]$ conan install ..`)

## Building the project using CMake

### Linux:
    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cp $OpenCV_DIR/../data/lbpcascades/lbpcascade_frontalface.xml .
    cp $OpenCV_DIR/../data/haarcascades/haarcascade_eye.xml .
    cp $OpenCV_DIR/../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml .
    cmake -D OpenCV_DIR=$OpenCV_DIR ..
    cmake --build .

### MacOSX (Xcode)
    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cp $OpenCV_DIR/../data/lbpcascades/lbpcascade_frontalface.xml .
    cp $OpenCV_DIR/../data/haarcascades/haarcascade_eye.xml .
    cp $OpenCV_DIR/../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml .
    cmake -G Xcode -D OpenCV_DIR=$OpenCV_DIR ..
    open WebcamFaceRec.xcodeproj

### Windows (MS Visual Studio)
    set OpenCV_DIR="C:\OpenCV\build"
    mkdir build
    cd build
    mkdir Debug
    mkdir Release
    copy %OpenCV_DIR%\..\data\lbpcascades\lbpcascade_frontalface.xml .\Debug\
    copy %OpenCV_DIR%\..\data\lbpcascades\lbpcascade_frontalface.xml .\Release\
    copy %OpenCV_DIR%\..\data\haarcascades\haarcascade_eye.xml .\Debug\
    copy %OpenCV_DIR%\..\data\haarcascades\haarcascade_eye.xml .\Release\
    copy %OpenCV_DIR%\..\data\haarcascades\haarcascade_eye_tree_eyeglasses.xml .\Debug\
    copy %OpenCV_DIR%\..\data\haarcascades\haarcascade_eye_tree_eyeglasses.xml .\Release\
    cmake -G "Visual Studio 9 2008" -D OpenCV_DIR=%OpenCV_DIR% ..
    start WebcamFaceRec.sln


## Running the project:

Just execute `WebcamFaceRec`.

If it says it can't find a Haar or LBP cascade XML file, copy those XML files from the OpenCV "data" folder to your current folder.

Warning for Visual Studio users: If you run the program directly in Visual Studio (eg: by clicking on "Debug->Start Without Debugging"), then Visual Studio will default to setting the "current folder" as the parent folder instead of the folder with "WebcamFaceRec.exe". So you might need to move or copy the XML file from the Debug / Release folder to the parent folder for it to run directly in Visual Studio. Or adjust your project properties so that it executes the program in the project output folder instead of the solution folder.

## Author
Shervin Emami<br/>
http://shervinemami.info/openCV.html
