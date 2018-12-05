# Chapter 2: Explore Structure from Motion with the SfM Module
Structure from Motion (SfM) is the process of recovering both the positions of cameras looking at the scene, as well as the sparse geometry of the scene. 
The motion between the cameras imposes geometric constraints that can help us recover the structure of objects, hence the process is called Structure from Motion. 
Since OpenCV v3.0+ a contributed ("contrib") module called sfm was added, that assists in performing end-to-end SfM processing from multiple images. 
In this chapter, we will learn how to use the SfM module to reconstruct a scene to a sparse point cloud including camera poses. 
Later we will also densify the point cloud, add many more points to it to make it dense, using an open Multi-View Stereo (MVS) package called OpenMVS.

## Requirements
* OpenCV 4 (compiled with the  sfm contrib module)
* Eigen v3.3.5+ (required by the sfm module and Ceres)
* Ceres solver v2+ (required by the sfm module)
* CMake 3.12+
* Boost v1.66+
* OpenMVS
* CGAL v4.12+ (required by OpenMVS)
* VCG (required by OpenMVS)

## Building instructions

### Building or installing third-party libraries
Follow the instructions on https://github.com/opencv/opencv_contrib to build OpenCV with the contrib modules.
You may only include the following contrib modules: `sfm`, `viz`

Build OpenMVS following these instructions: https://github.com/cdcseacave/openMVS/wiki/Building.
These also include the installation instructions for Ceres, Boost, Eigen and CGAL.

Example for building OpenMVS: (OpenMVS already has a `build` dir, from which CMake should be called)
```
$ cd <openMVS dir>/build
$ cmake .. -DOpenCV_DIR=<opencv4 dir>/build -DCGAL_ROOT=<cgal4 dir>/lib/cmake/ -DVCG_DIR=<vcg dir> -DCMAKE_BUILD_TYPE=Release
$ make
```

### Building project
```
$ mkdir build && cd build
$ cmake .. -DOpenCV_DIR=<opencv dir>/build -DOpenMVS_DIR=<openMVS dir>/build
$ make
```

## Running instructions
```
Usage: ch2_sfm [params] dir 

	-?, -h, --help (value:true)
		help message
	--cloud
		Save reconstruction to a point cloud file (PLY, XYZ and OBJ). Provide filename
	--debug (value:false)
		Save debug visualizations to files?
	--mrate (value:0.5)
		Survival rate of matches to consider image pair success
	--mvs
		Save reconstruction to an .mvs file. Provide filename
	--viz (value:false)
		Visualize the sparse point cloud reconstruction?

	dir (value:.)
		directory with image files for reconstruction
```

Example:
```
$ ./ch2_sfm --viz=true --mvs=out.mvs ../crazyhorse/
$ <openMVS dir>/build/bin/DensifyPointCloud -i out.mvs
$ <openMVS dir>/build/bin/Viewer -i out_dense.mvs
```

## Author
Roy Shilkrot <br/>
roy.shil@gmail.com <br/>
http://www.morethantechnical.com
