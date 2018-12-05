# Chapter 3: Face Landmark and Pose with the Face Module

Face landmark detection is the process of finding points of interest on the image of a human face. 
It recently saw a spur of interest in the computer vision community, as it has many compelling applications, for example—detecting emotion through facial gestures, estimating gaze direction, changing facial appearance (face swap), augmenting faces with graphics, and puppeteering of virtual characters. 
In this chapter we will discuss the process of face landmark (also known as facemark) detection using the `cv::face` module, which provides an API for inference as well as training of a facemark detector. 
We will see how to apply the facemark detector to finding the direction of the face in 3D.

## Requirements
* OpenCV v4 (compiled with the face contrib module)
* Boost v1.66+

A detector model, downloadable from: https://raw.githubusercontent.com/kurnianggoro/GSOC2017/master/data/lbfmodel.yaml

Input video with facemark annotations, such as in the 300-VW dataset: https://ibug.doc.ic.ac.uk/resources/300-VW/

## Building instructions
Follow the instructions on https://github.com/opencv/opencv_contrib to build OpenCV with the contrib modules.
You may only include the following contrib modules: `face`, `xfeatures2d`, `photo`

Building the project:
```
$ mkdir build && cd build
$ cmake .. -DOpenCV_DIR=<opencv4 dir>/build
$ make
```

## Running instructions
```
$ ./ch3_face -c=<opencv4 dir>/data/haarcascades/haarcascade_frontalface_default.xml -f=lbfmodel.yaml -v="<300vw dataset dir>/001/"
```

## Author
Roy Shilkrot <br/>
roy.shil@gmail.com <br/>
http://www.morethantechnical.com
