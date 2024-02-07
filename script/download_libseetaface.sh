#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
SEETA_FACE_DIR=$DEPS_DIR/SeetaFace6Open

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $SEETA_FACE_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone --recursive https://github.com/SeetaFace6Open/index.git $SEETA_FACE_DIR

cd $SEETA_FACE_DIR
git checkout a32e2faa0694c0f841ace4df9ead0407b78363c6

echo "apply OpenRoleZoo patch"

function apply_path() {
    echo "[*] apply path ${1} ${2}"
    cd $SEETA_FACE_DIR/${1}
    git apply --check $GIT_ROOT_DIR/patch/${2}
    git apply $GIT_ROOT_DIR/patch/${2}
}

apply_path "OpenRoleZoo" "pot.h.patch"

# cd $SEETA_FACE_DIR/OpenRoleZoo

# git apply --check $GIT_ROOT_DIR/patch/pot.h.patch
# git apply $GIT_ROOT_DIR/patch/pot.h.patch


case $(uname | tr '[:upper:]' '[:lower:]') in
  linux*)
    #
    ;;
  darwin*)
    # macOS
    apply_path "TenniS" "tennis_macos.patch"
    ;;
  msys*)
    # windows
    ;;
  *)
    echo "[*] uname: $(uname)"
    ;;
esac


cd $CUR_DIR
