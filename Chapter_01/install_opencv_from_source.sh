#!/usr/bin/env bash

# This script will build OpenCV from sources using the latest HEAD on github.
# Assuming a linux environment with `apt-get` available.

OPENCV_VERSION="master"
INSTALL_DIR="opencv-from-source"

# Clean and create a directory for installation
rm -rf ${INSTALL_DIR} && mkdir ${INSTALL_DIR}
cd ${INSTALL_DIR}

# Install dependencies
sudo apt-get -y remove x264 libx264-dev
sudo apt-get -y install build-essential checkinstall cmake pkg-config yasm \
    git gfortran libjpeg8-dev libjasper-dev libpng12-dev \
    libtiff5-dev libtiff-dev libavcodec-dev libavformat-dev libswscale-dev \
    libdc1394-22-dev libxine2-dev libv4l-dev libgstreamer0.10-dev \
    libgstreamer-plugins-base0.10-dev libgtk2.0-dev libtbb-dev qt5-default \
    libatlas-base-dev libmp3lame-dev libtheora-dev libvorbis-dev libxvidcore-dev \
    libx264-dev libopencore-amrnb-dev libopencore-amrwb-dev libavresample-dev \
    x264 v4l-utils

pushd /usr/include/linux
sudo ln -s -f ../libv4l1-videodev.h videodev.h
popd

# Download source
git clone https://github.com/opencv/opencv.git
mkdir -p opencv/build && cd opencv/build

# Run CMake to generate makefiles
cmake -D CMAKE_BUILD_TYPE=RELEASE \
            -D CMAKE_INSTALL_PREFIX=/usr/local/ \
            -D INSTALL_C_EXAMPLES=OFF \
            -D WITH_TBB=ON \
            -D WITH_V4L=ON \
            -D WITH_QT=ON \
            -D WITH_OPENGL=ON \
            -D BUILD_EXAMPLES=OFF \
            -D BUILD_PERF_TESTS=OFF \
            -D BUILD_TESTS=OFF \
            -D BUILD_DOCS=OFF \
            ..

# Build and install
make -j$(nproc)
make install
