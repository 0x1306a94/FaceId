#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
LIBHV_DIR=$DEPS_DIR/libhv
LIBHV_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/libhv
LIBHV_BUILD_DIR=$LIBHV_DIR/cmake_build

if [[ -d "$LIBHV_BUILD_DIR" ]]; then
    rm -rf $LIBHV_BUILD_DIR
fi

if [[ -d "$LIBHV_INSTALL_DIR" ]]; then
    rm -rf $LIBHV_INSTALL_DIR
fi

cd $LIBHV_DIR

cmake -S . -B $LIBHV_BUILD_DIR \
-DCMAKE_BUILD_TYPE=Release \
-DWITH_OPENSSL=OFF \
-DBUILD_EXAMPLES=OFF \
-DBUILD_UNITTEST=OFF \
-DCMAKE_INSTALL_PREFIX="$LIBHV_INSTALL_DIR"

cmake --build $LIBHV_BUILD_DIR -j8
cmake --install $LIBHV_BUILD_DIR

# mkdir -p $LIBHV_INSTALL_DIR

# cp -rf $LIBHV_BUILD_DIR/lib $LIBHV_INSTALL_DIR
# cp -rf $LIBHV_BUILD_DIR/include $LIBHV_INSTALL_DIR

cd $CUR_DIR