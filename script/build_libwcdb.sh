#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
WCDB_DIR=$DEPS_DIR/wcdb
WCDB_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/wcdb
WCDB_BUILD_DIR=$WCDB_DIR/cmake_build

if [[ -d "$WCDB_BUILD_DIR" ]]; then
    rm -rf $WCDB_BUILD_DIR
fi

if [[ -d "$WCDB_INSTALL_DIR" ]]; then
    rm -rf $WCDB_INSTALL_DIR
fi

cd $WCDB_DIR

cmake -S $WCDB_DIR/tools/version -B $WCDB_BUILD_DIR \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX="$WCDB_INSTALL_DIR"

cmake --build $WCDB_BUILD_DIR -j8
cmake --install $WCDB_BUILD_DIR

cd $CUR_DIR