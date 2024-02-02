#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
SPDLOG_DIR=$DEPS_DIR/spdlog

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $SPDLOG_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone -b v1.11.0 https://github.com/gabime/spdlog $SPDLOG_DIR

cd $CUR_DIR