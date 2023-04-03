#!/bin/bash

cd $HOME
mkdir src
cd src

echo "Staging eccodes source files to $(pwd)"
curl -OL https://github.com/ecmwf/eccodes/archive/refs/tags/${ECCODES_VERSION}.tar.gz
tar -xzf ${ECCODES_VERSION}.tar.gz
cd eccodes-${ECCODES_VERSION}

mkdir build
if [[ -z ${OPENJPEG_INCLUDE_DIR} ]]; then
    cmake . -B build -DCMAKE_BUILD_TYPE=Release \
                    -DENABLE_NETCDF=OFF \
                    -DENABLE_JPG=ON \
                    -DENABLE_PNG=ON \
                    -DENABLE_AEC=ON \
                    -DENABLE_PYTHON=OFF \
                    -DENABLE_FORTRAN=OFF \
                    -DENABLE_EXAMPLES=OFF \
                    -DENABLE_JPG_LIBJASPER=OFF \
                    -DENABLE_INSTALL_ECCODES_SAMPLES=OFF
else
    echo "OPENJPEG_INCLUDE_DIR: ${OPENJPEG_INCLUDE_DIR}"
    cmake . -B build -DCMAKE_BUILD_TYPE=Release \
                    -DENABLE_NETCDF=OFF \
                    -DENABLE_JPG=ON \
                    -DENABLE_PNG=ON \
                    -DENABLE_AEC=ON \
                    -DENABLE_PYTHON=OFF \
                    -DENABLE_FORTRAN=OFF \
                    -DENABLE_EXAMPLES=OFF \
                    -DENABLE_INSTALL_ECCODES_SAMPLES=OFF \
                    -DENABLE_JPG_LIBOPENJPEG=ON \
                    -DENABLE_JPG_LIBJASPER=OFF \
                    -DOPENJPEG_INCLUDE_DIR=${OPENJPEG_INCLUDE_DIR}
fi

cmake --build ./build

sudo cmake --install ./build

cd $GITHUB_WORKSPACE
