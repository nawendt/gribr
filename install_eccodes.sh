#!/bin/bash

cd $GITHUB_WORKSPACE
echo Staging dependency files to $(pwd)
curl -o eccodes-${ECCODES_VERSION}-Source.tar.gz \
		https://confluence.ecmwf.int/download/attachments/45757960/eccodes-${ECCODES_VERSION}-Source.tar.gz?api=v2
tar -xzf eccodes-${ECCODES_VERSION}-Source.tar.gz
cd eccodes-${ECCODES_VERSION}-Source

mkdir build

cmake . -B build -DCMAKE_BUILD_TYPE=Release \
                 -DENABLE_NETCDF=OFF \
                 -DENABLE_JPG=ON \
                 -DENABLE_PNG=ON \
                 -DENABLE_AEC=ON \
                 -DENABLE_PYTHON=OFF \
                 -DENABLE_FORTRAN=OFF \
                 -DENABLE_EXAMPLES=OFF

cmake --build ./build

sudo cmake --install ./build
