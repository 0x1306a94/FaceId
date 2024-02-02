#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
LIBHV_DIR=$DEPS_DIR/libhv

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $LIBHV_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone -b v1.3.0 https://github.com/ithewei/libhv $LIBHV_DIR

cd $CUR_DIR