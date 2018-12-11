# Chapter 1: Cartoonifier, for Desktop or Android.

This chapter will show how to write some image processing filters for desktops and for small embedded systems such as Raspberry Pi. 
First, we develop it for the desktop (in C/C++) and then port the project to Raspberry Pi, since this is the recommended scenario when developing for embedded devices.

This folder contains the Cartoonifier project, that can be used on desktop
(works on Windows, Mac, Linux, etc) or embedded (works on Raspberry Pi, etc).

The file "main.cpp" is for the OpenCV user interface and "cartoon.cpp" is for
the image processing.
It includes a CMake project file to allow building with different compilers &
versions for Windows, Mac, Linux, etc.


## Building the Cartoonifier project using CMake

### Embedded (Raspberry Pi, etc):
    Follow the steps recommended in Chapter 1 of the book.

### Desktop Linux:
    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cmake -D OpenCV_DIR=$OpenCV_DIR ..
    make 

### MacOSX (Xcode):
    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cmake -G Xcode -D OpenCV_DIR=$OpenCV_DIR ..
    open Cartoonifier.xcodeproj

### Windows (MS Visual Studio):
    set OpenCV_DIR="C:\OpenCV\build"
    mkdir build
    cd build
    cmake -G "Visual Studio 9 2008" -D OpenCV_DIR=%OpenCV_DIR% ..
    start Cartoonifier.sln 

    
## Running the project:

(From the "Cartoonifier" folder):
Just execute `Cartoonifier`, such as `./Cartoonifier` in Linux.

## Author
Shervin Emami<br/>
http://shervinemami.info/openCV.html
