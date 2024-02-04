#!/bin/sh -e

set -e

CUR_DIR=$PWD

DEB_VERSION="1.0.0"
DEB_NAME="face-id-server-${DEB_VERSION}-amd64.deb"

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
BUILD_WORK_DIR=$GIT_ROOT_DIR/build_deb_work
CMAKE_BUILD_DIR=$BUILD_WORK_DIR/cmake_build
CMAKE_INSTALL_DIR=$BUILD_WORK_DIR/cmake_install
DEB_WORK_DIR=$BUILD_WORK_DIR/deb_work
DEB_BIN_DIR=$DEB_WORK_DIR/opt/face_id_server

if [ ! -d "${BUILD_WORK_DIR}" ]; then
    mkdir -p $BUILD_WORK_DIR
fi

if [ -d "${CMAKE_BUILD_DIR}" ]; then
    rm -rf $CMAKE_BUILD_DIR
fi

if [ -d "${CMAKE_INSTALL_DIR}" ]; then
    rm -rf $CMAKE_INSTALL_DIR
fi

if [ -d "${DEB_WORK_DIR}" ]; then
    echo "rm $DEB_WORK_DIR"
    rm -rf $DEB_WORK_DIR
fi

cmake -S . -B $CMAKE_BUILD_DIR \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX="$CMAKE_INSTALL_DIR" \
-DDEFAULT_FACE_ID_DATA_DIR=/data/face_id_server \
-DDEFAULT_FACE_ID_MODELE_DIR=/opt/face_id_server/sf3.0_models

cmake --build $CMAKE_BUILD_DIR -j8
cmake --install $CMAKE_BUILD_DIR

# mv $CMAKE_INSTALL_DIR/lib/libopencv_core.so.4.5.3 $CMAKE_INSTALL_DIR/lib/libopencv_core.so.4.5
# mv $CMAKE_INSTALL_DIR/lib/libopencv_imgproc.so.4.5.3 $CMAKE_INSTALL_DIR/lib/libopencv_imgproc.so.4.5
# mv $CMAKE_INSTALL_DIR/lib/libopencv_imgcodecs.so.4.5.3 $CMAKE_INSTALL_DIR/lib/libopencv_imgcodecs.so.4.5
mkdir -p $DEB_BIN_DIR

cp -rf $GIT_ROOT_DIR/deb_template/* $DEB_WORK_DIR
cp -rf $CMAKE_INSTALL_DIR/* $DEB_BIN_DIR

echo "/opt/face_id_server/lib" > $DEB_BIN_DIR/config/face_id_server.conf

INSTALLED_SIZE=$(du -ks $DEB_BIN_DIR | cut -f 1)
sed -i "s/\${Version}/${DEB_VERSION}/g" $DEB_WORK_DIR/DEBIAN/control
sed -i "s/\${Installed-Size}/${INSTALLED_SIZE}/g" $DEB_WORK_DIR/DEBIAN/control

dpkg-deb --root-owner-group -b $DEB_WORK_DIR $BUILD_WORK_DIR/$DEB_NAME