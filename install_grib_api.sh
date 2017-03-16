#!/bin/bash

cd $HOME
echo Staging dependency files to $(pwd)
curl -o grib_api-${GRIB_API_VERSION}-Source.tar.gz https://software.ecmwf.int/wiki/download/attachments/3473437/grib_api-${GRIB_API_VERSION}-Source.tar.gz?api=v2
tar -xzf grib_api-${GRIB_API_VERSION}-Source.tar.gz
cd grib_api-${GRIB_API_VERSION}-Source
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DENABLE_NETCDF=OFF -DENABLE_JPG=ON -DENABLE_PNG=ON -DENABLE_PYTHON=OFF -DENABLE_FORTRAN=OFF -DENABLE_EXAMPLES=OFF
make && sudo make install
cd $TRAVIS_BUILD_DIR