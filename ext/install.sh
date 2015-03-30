#!/bin/bash

basedir=$(cd $(dirname $0); pwd)

check_executable () {
  command -v $executable >/dev/null 2>&1 || {
    echo >&2 "Unable to find '$1', it is either not installed or not in the PATH.  Aborting."
    exit 1
  }
}

check_executable wget
check_executable libtool
check_executable cmake
check_executable xz

set -ex

mkdir /tmp/php-driver-installation
pushd /tmp/php-driver-installation

mkdir build

echo "Compiling libuv..."
wget http://libuv.org/dist/v1.4.2/libuv-v1.4.2.tar.gz
tar -xzvf libuv-v1.4.2.tar.gz

pushd libuv-v1.4.2
sh autogen.sh
./configure --disable-shared --prefix=/
make
make install DESTDIR=$PWD/../build
popd

rm -Rf libuv-v1.4.2 libuv-v1.4.2.tar.gz

echo "Compiling gmp..."
wget https://ftp.gnu.org/gnu/gmp/gmp-6.0.0a.tar.xz
unxz gmp-6.0.0a.tar.xz
tar -xf gmp-6.0.0a.tar

pushd gmp-6.0.0
./configure --disable-shared --prefix=/
make
make install DESTDIR=$PWD/../build
popd

rm -Rf gmp-6.0.0 gmp-6.0.0a.tar gmp-6.0.0a.tar.xz

echo "Compiling cpp-driver..."
mkdir cpp-driver
pushd cpp-driver
CMAKE_PREFIX_PATH=$PWD/../build cmake -DCMAKE_INSTALL_PREFIX:PATH=/ -DCASS_BUILD_STATIC=ON -DCASS_BUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_LIBDIR:PATH=lib $basedir/../lib/cpp-driver/
make
make install DESTDIR=$PWD/../build
popd
rm -Rf cpp-driver

mv build/lib/libcassandra_static.a build/lib/libcassandra.a

popd

echo "Compiling extension..."
phpize
./configure --with-cassandra=/tmp/php-driver-installation/build --with-gmp=/tmp/php-driver-installation/build --with-libdir=lib
make




# brew install libuv cmake gmp
#
# echo "Installing cpp-driver..."
# brew install https://raw.githubusercontent.com/datastax/cpp-driver/master/packaging/homebrew/cassandra-cpp-driver.rb
#
# echo "Compiling extension..."
# phpize
# ./configure --with-cassandra
# make
# make test
# make install
