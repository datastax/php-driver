#!/bin/bash
git clone https://github.com/datastax/cpp-driver.git
mkdir build
cd build
builddir=`pwd`
cmake -DCMAKE_CXX_FLAGS="-fPIC" -DCMAKE_INSTALL_PREFIX:PATH=$builddir -DCASS_BUILD_STATIC=ON \
  -DCASS_BUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=RELEASE -DCASS_USE_ZLIB=ON \
  -DCMAKE_INSTALL_LIBDIR:PATH=lib ../cpp-driver/
make
make install
mv lib/libcassandra_static.a lib/libcassandra.a
