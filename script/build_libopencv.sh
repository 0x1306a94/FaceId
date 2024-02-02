#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
OPENCV_VERSION="4.5.3"
OPENCV_DIR=$DEPS_DIR/opencv_$OPENCV_VERSION
OPENCV_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/opencv_$OPENCV_VERSION
OPENCV_BUILD_DIR=$OPENCV_DIR/cmake_build

if [[ -d "$OPENCV_BUILD_DIR" ]]; then
    rm -rf $OPENCV_BUILD_DIR
fi

if [[ -d "$OPENCV_INSTALL_DIR" ]]; then
    rm -rf $OPENCV_INSTALL_DIR
fi

cd $OPENCV_DIR

BUILD_SHARED_LIBS=OFF
cmake -S . -B $OPENCV_BUILD_DIR \
-DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} \
-DCMAKE_BUILD_TYPE=Release \
-DPENCV_GENERATE_PKGCONFIG=ON \
-DBUILD_JPEG=NO \
-DBUILD_PNG=NO \
-DBUILD_WEBP=ON \
-DWITH_AVFOUNDATION=OFF \
-DWITH_FFMPEG=OFF \
-DWITH_PROTOBUF=OFF \
-DBUILD_TESTS=OFF \
-DBUILD_opencv_python2=OFF \
-DBUILD_opencv_python3=OFF \
-DCMAKE_INSTALL_PREFIX="$OPENCV_INSTALL_DIR"

# cmake --build $OPENCV_BUILD_DIR -j8
cmake --build $OPENCV_BUILD_DIR --target opencv_core --target opencv_imgcodecs --target opencv_imgproc -j8
cmake --install $OPENCV_BUILD_DIR 

# mkdir -p $OPENCV_INSTALL_DIR

# cp -rf $OPENCV_BUILD_DIR/lib $LIBHV_INSTALL_DIR
# cp -rf $OPENCV_BUILD_DIR/include $LIBHV_INSTALL_DIR

cd $CUR_DIR