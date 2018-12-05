# Chapter 2: Explore Structure from Motion with the SfM Module
Structure from Motion (SfM) is the process of recovering both the positions of cameras looking at the scene, as well as the sparse geometry of the scene. 
The motion between the cameras imposes geometric constraints that can help us recover the structure of objects, hence the process is called Structure from Motion. 
Since OpenCV v3.0+ a contributed ("contrib") module called sfm was added, that assists in performing end-to-end SfM processing from multiple images. 
In this chapter, we will learn how to use the SfM module to reconstruct a scene to a sparse point cloud including camera poses. 
Later we will also densify the point cloud, add many more points to it to make it dense, using an open Multi-View Stereo (MVS) package called OpenMVS.

## Requirements
* OpenCV 4 (compiled with the  sfm contrib module)
* Eigen v3.3+ (required by the sfm module)
* Ceres solver v2+ (required by the sfm module)
* CMake 3.12+
* Boost v1.66+
* OpenMVS
* CGAL v4.12+ (required by OpenMVS)

## Building instructions
TODO

## Running instructions
TODO

## Author
Roy Shilkrot <br/>
roy.shil@gmail.com <br/>
http://www.morethantechnical.com
