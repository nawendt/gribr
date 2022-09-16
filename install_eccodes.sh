#!/bin/bash

cd $HOME
mkdir src
cd src

echo "Staging eccodes source files to $(pwd)"
curl -o eccodes-${ECCODES_VERSION}-Source.tar.gz \
		https://confluence.ecmwf.int/download/attachments/45757960/eccodes-${ECCODES_VERSION}-Source.tar.gz?api=v2
tar -xzf eccodes-${ECCODES_VERSION}-Source.tar.gz
cd eccodes-${ECCODES_VERSION}-Source

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
