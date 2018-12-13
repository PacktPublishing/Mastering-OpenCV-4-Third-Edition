# Chapter 7: Android Camera Calibration and AR using the ARUco Module

One of the most sought after application for mobile computer vision is augmented reality (AR). 
Blending real and virtual worlds has applications in entertainment and gaming, medical and healthcare, industry and defense, and many more. 
In this chapter we will learn how to implement an AR application from scratch in the Android ecosystem, by using OpenCV's **ARUco** contrib module, **Android's Camera2 APIs** as well as the **JMonkeyEngine 3D** game engine. 
However first we will begin with simply calibrating our Android device's camera using ARUco's ChArUco calibration board, which provides a more robust alternative to OpenCV's calib3d chessboards.

## Requirements
* OpenCV v3 or v4 Android SDK compiled with ArUco contrib module: https://github.com/Mainvooid/opencv-android-sdk-with-contrib
* Android Studio v3.2+
* Android device running Android OS v6.0+

## Building instructions
This project must be built with the Android Studio IDE.
After installing Android Studio, simply open the android project in this directory.

To complete the build, you must point the project configuration to find the OpenCV+contrib Android SDK you obtained earlier.

In the `gradle.settings` file, change the following line to match your directory setup: (make sure to use a relative path)

    opencvsdk=../../opencv-android-sdk-with-contrib
  

## Running instructions
Run the project as a normal Android app from the Android Studio.

## Author
Roy Shilkrot <br/>
roy.shil@gmail.com <br/>
http://www.morethantechnical.com
