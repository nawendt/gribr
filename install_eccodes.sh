#!/bin/bash

cd $HOME
echo Staging dependency files to $(pwd)
curl -o eccodes-${ECCODES_VERSION}-Source.tar.gz \
		https://confluence.ecmwf.int/download/attachments/45757960/eccodes-${ECCODES_VERSION}-Source.tar.gz?api=v2
tar -xzf eccodes-${ECCODES_VERSION}-Source.tar.gz
cd eccodes-${ECCODES_VERSION}-Source

mkdir build && cd build

if [[ $TRAVIS_OS_NAME == "linux" ]]; then
	AEC=ON
else
	AEC=OFF
fi

if [[ -n ${OPENJPEG_INCLUDE_DIR} ]]; then
	cmake .. -DCMAKE_BUILD_TYPE=Release \
	         -DENABLE_NETCDF=OFF \
	         -DENABLE_JPG=ON \
	         -DENABLE_PNG=ON \
			 -DENABLE_AEC=${AEC} \
	         -DENABLE_PYTHON=OFF \
	         -DENABLE_FORTRAN=OFF \
	         -DENABLE_EXAMPLES=OFF \
	         -DOPENJPEG_INCLUDE_DIR=${OPENJPEG_INCLUDE_DIR}
else
	cmake .. -DCMAKE_BUILD_TYPE=Release \
	         -DENABLE_NETCDF=OFF \
	         -DENABLE_JPG=ON \
	         -DENABLE_PNG=ON \
			 -DENABLE_AEC=${AEC} \
	         -DENABLE_PYTHON=OFF \
	         -DENABLE_FORTRAN=OFF \
	         -DENABLE_EXAMPLES=OFF
fi

make && sudo make install
cd $TRAVIS_BUILD_DIR
