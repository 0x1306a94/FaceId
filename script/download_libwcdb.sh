#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
WCDB_DIR=$DEPS_DIR/wcdb

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $WCDB_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone -b v2.0.4 https://github.com/Tencent/wcdb $WCDB_DIR
cd $WCDB_DIR
git submodule update --init sqlcipher

cd $CUR_DIR