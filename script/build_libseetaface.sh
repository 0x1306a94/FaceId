#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
SEETA_FACE_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/SeetaFace6Open
SEETA_FACE_DIR=$DEPS_DIR/SeetaFace6Open


if [[ -d "$SEETA_FACE_INSTALL_DIR" ]]; then
    rm -rf $SEETA_FACE_INSTALL_DIR
fi

BUILD_TARGETS=(OpenRoleZoo SeetaAuthorize TenniS FaceBoxes Landmarker FaceRecognizer6 FaceTracker6 SeetaEyeStateDetector SeetaGenderPredictor SeetaMaskDetector FaceAntiSpoofingX6 PoseEstimator6 QualityAssessor3 SeetaAgePredictor)

for TARGET in ${BUILD_TARGETS[@]} 
do
    echo "TARGET: ${TARGET}"
    TARGET_SOURCE_DIR=$SEETA_FACE_DIR/$TARGET
    echo "TARGET_SOURCE_DIR: ${TARGET_SOURCE_DIR}"
    cd $TARGET_SOURCE_DIR/craft
    chmod +x build.linux.x64.sh
    ./build.linux.x64.sh
done

mkdir -p $SEETA_FACE_INSTALL_DIR

cp -rf $SEETA_FACE_DIR/build/* $SEETA_FACE_INSTALL_DIR

cd $CUR_DIR