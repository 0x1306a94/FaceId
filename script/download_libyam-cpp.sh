#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
YAML_CPP_DIR=$DEPS_DIR/yaml-cpp

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $YAML_CPP_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone -b yaml-cpp-0.7.0 https://github.com/jbeder/yaml-cpp $YAML_CPP_DIR

cd $CUR_DIR