#!/bin/bash

basedir=$(cd $(dirname $0); pwd)

check_executable () {
  executable=$1

  command -v $executable >/dev/null 2>&1 || {
    echo >&2 "Unable to find '$executable', it is either not installed or not in the PATH.  Aborting."
    exit 1
  }
}

download_archive () {
  url=$1
  md5=$2
  file=$(basename $url)
  wget $url

  if [ $md5 != $(md5sum $file | cut -f 1 -d " ") ]; then
    echo "Bad file '$file', md5 sums mismatch.  Aborting."
    exit 1
  fi
}

check_executable wget
check_executable md5sum
check_executable libtool
check_executable cmake
check_executable xz

set -ex

mkdir /tmp/php-driver-installation
pushd /tmp/php-driver-installation

mkdir build
builddir=$(cd build; pwd)

echo "Compiling libuv..."
download_archive http://libuv.org/dist/v1.4.2/libuv-v1.4.2.tar.gz "d116fbe695157b799fb8805477e24eff"
tar -xzvf libuv-v1.4.2.tar.gz

pushd libuv-v1.4.2
sh autogen.sh
./configure --disable-shared --prefix=$builddir
make
make install
popd

rm -Rf libuv-v1.4.2 libuv-v1.4.2.tar.gz

echo "Compiling cpp-driver..."
mkdir cpp-driver
pushd cpp-driver
LIBUV_ROOT_DIR=$builddir CMAKE_PREFIX_PATH=$builddir cmake -DCMAKE_INSTALL_PREFIX:PATH=$builddir -DCASS_BUILD_STATIC=ON -DCASS_BUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_LIBDIR:PATH=lib $basedir/../lib/cpp-driver/
make
make install
popd
rm -Rf cpp-driver

mv build/lib/libcassandra_static.a build/lib/libcassandra.a

echo "Compiling gmp..."
download_archive https://ftp.gnu.org/gnu/gmp/gmp-6.0.0a.tar.xz "1e6da4e434553d2811437aa42c7f7c76"
unxz gmp-6.0.0a.tar.xz
tar -xf gmp-6.0.0a.tar

pushd gmp-6.0.0
./configure --disable-shared --prefix=$builddir
make
make install
popd

rm -Rf gmp-6.0.0 gmp-6.0.0a.tar gmp-6.0.0a.tar.xz

popd

echo "Compiling extension..."
phpize
./configure --with-cassandra=/tmp/php-driver-installation/build --with-gmp=/tmp/php-driver-installation/build --with-libdir=lib CFLAGS="-pthread" LDFLAGS=-L$builddir/lib LIBS="-luv -lstdc++"
make
sudo make install
sudo echo "# DataStax PHP Driver\nextension=cassandra.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
