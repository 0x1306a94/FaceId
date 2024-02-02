#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
YAML_CPP_DIR=$DEPS_DIR/yaml-cpp
YAML_CPP_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/yaml-cpp
YAML_CPP_BUILD_DIR=$YAML_CPP_DIR/cmake_build

if [[ -d "$YAML_CPP_BUILD_DIR" ]]; then
    rm -rf $YAML_CPP_BUILD_DIR
fi

if [[ -d "$YAML_CPP_INSTALL_DIR" ]]; then
    rm -rf $YAML_CPP_INSTALL_DIR
fi

# mkdir -p $YAML_CPP_INSTALL_DIR/{lib,include}
cd $YAML_CPP_DIR

cmake -S . -B $YAML_CPP_BUILD_DIR \
-DCMAKE_BUILD_TYPE=Release \
-DYAML_BUILD_SHARED_LIBS=OFF \
-DYAML_CPP_BUILD_TOOLS=OFF \
-DYAML_CPP_BUILD_TESTS=OFF \
-DCMAKE_INSTALL_PREFIX="$YAML_CPP_INSTALL_DIR"

cmake --build $YAML_CPP_BUILD_DIR -j8
cmake --install $YAML_CPP_BUILD_DIR

# mkdir -p $YAML_CPP_INSTALL_DIR

# cp -rf $YAML_CPP_BUILD_DIR/libyaml-cpp.a $YAML_CPP_INSTALL_DIR/lib
# cp -rf $YAML_CPP_DIR/include $YAML_CPP_INSTALL_DIR

cd $CUR_DIR