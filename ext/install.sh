#!/bin/bash

basedir=$(cd $(dirname $0); pwd)

check_executable () {
  for executable; do
    command -v $executable >/dev/null 2>&1 || {
      echo >&2 "Unable to find '$executable', make sure it is installed and is in the PATH.  Aborting."
      exit 1
    }
  done
}

check_executable cmake grep sed

set -ex

rm -Rf /tmp/php-driver-installation
mkdir /tmp/php-driver-installation
pushd /tmp/php-driver-installation

mkdir build
builddir=$(cd build; pwd)

echo "Compiling cpp-driver..."
mkdir cpp-driver
pushd cpp-driver
cmake -DCMAKE_INSTALL_PREFIX:PATH=$builddir -DCASS_BUILD_STATIC=ON \
  -DCASS_BUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=RELEASE -DCASS_USE_ZLIB=ON \
  -DCMAKE_INSTALL_LIBDIR:PATH=lib $basedir/../lib/cpp-driver/
make
make install
popd
rm -Rf cpp-driver
mv build/lib/libcassandra_static.a build/lib/libcassandra.a

popd

echo "Compiling and installing the extension..."
phpize

case $(uname -s) in
  Linux)
    CFLAGS="-pthread"
    LDFLAGS="-L$builddir/lib"
    LIBS="-lssl -lz -luv -lgmp -lstdc++"
    ;;
  Darwin)
    CFLAGS=
    LDFLAGS="-L$builddir/lib"
    LIBS="-lssl -lz -luv -lgmp -lstdc++"
    ;;
esac

CFLAGS=$CFLAGS LDFLAGS=$LDFLAGS LIBS=$LIBS ./configure \
  --with-cassandra=$builddir --with-libdir=lib
make
make install

rm -Rf /tmp/php-driver-installation
