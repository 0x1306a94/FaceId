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
    mkdir -p $WCDB_INSTALL_DIR
fi

cd $WCDB_DIR

$WCDB_DIR/tools/version/build.sh -p macOS -l Cpp -d $WCDB_BUILD_DIR --disable-bitcode
cp -Rf $WCDB_BUILD_DIR/WCDBCpp.framework $WCDB_INSTALL_DIR
cd $CUR_DIR