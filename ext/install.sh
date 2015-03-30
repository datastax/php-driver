#!/bin/bash

basedir=$(cd $(dirname $0); pwd)

check_executable () {
  for executable; do
    command -v $executable >/dev/null 2>&1 || {
      echo >&2 "Unable to find '$executable', it is either not installed or not in the PATH.  Aborting."
      exit 1
    }
  done
}

check_executable cmake grep sed

set -ex

rm -Rf /tmp/php-driver-installation/
mkdir /tmp/php-driver-installation
pushd /tmp/php-driver-installation

mkdir build
builddir=$(cd build; pwd)

echo "Compiling cpp-driver..."
mkdir cpp-driver
pushd cpp-driver
cmake -DCMAKE_INSTALL_PREFIX:PATH=$builddir -DCASS_BUILD_STATIC=ON -DCASS_BUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_LIBDIR:PATH=lib -DCASS_USE_ZLIB=ON $basedir/../lib/cpp-driver/
make
make install
popd
rm -Rf cpp-driver
mv build/lib/libcassandra_static.a build/lib/libcassandra.a

popd

echo "Compiling and installing the extension..."
phpize
# LDFLAGS="-L$builddir/lib" LIBS="-luv -lgmp -lstdc++" ./configure --with-cassandra=/tmp/php-driver-installation/build --with-libdir=lib
CFLAGS="-pthread" LDFLAGS="-L$builddir/lib" LIBS="-lssl -lz -luv -lgmp -lstdc++" ./configure --with-cassandra=/tmp/php-driver-installation/build --with-gmp=/tmp/php-driver-installation/build --with-libdir=lib
make
sudo make install
# sh -c 'echo "# DataStax PHP Driver\nextension=cassandra.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`'
rm -Rf /tmp/php-driver-installation/
