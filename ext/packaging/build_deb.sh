#!/bin/bash

# Dependencies
# sudo apt-get install git cmake curl build-essential debhelper devscripts autoconf automake libtool

function check_command {
  command=$1
  package=$2
  if ! type "$command" > /dev/null 2>&1; then
    echo "Missing command '$command', run: apt-get install $package"
    exit 1
  fi
}

function header_version {
  read -d ''  version_script << 'EOF'
  BEGIN { major="?"; minor="?"; patch="?" }
  /PHP_DRIVER_MAJOR/ { major=$3 }
  /PHP_DRIVER_MINOR/ { minor=$3 }
  /PHP_DRIVER_RELEASE/ { patch=$3 }
  END {
    printf "%s.%s.%s", major, minor, patch
  }
EOF
  version=$(grep '#define[ \t]\+PHP_DRIVER_\(MAJOR\|MINOR\|RELEASE\)' $1 | awk "$version_script")
  if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+([a-zA-Z0-9_\-]+)?$ ]]; then
    echo "Unable to extract version from $1"
    exit 1
  fi
  echo "$version"
}

function header_version_full {
  read -d ''  version_script << 'EOF'
  BEGIN { major="?"; minor="?"; patch="?" }
  /PHP_DRIVER_MAJOR/ { major=$3 }
  /PHP_DRIVER_MINOR/ { minor=$3 }
  /PHP_DRIVER_RELEASE/ { patch=$3 }
  /PHP_DRIVER_STABILITY/ { suffix=$3; gsub(/"/, "", suffix) }
  END {
    if (length(suffix) > 0)
      printf "%s.%s.%s~%s", major, minor, patch, suffix
    else
      printf "%s.%s.%s", major, minor, patch
  }
EOF
  version=$(grep '#define[ \t]\+PHP_DRIVER_\(MAJOR\|MINOR\|RELEASE\|STABILITY\)' $1 | awk "$version_script")
  if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+(~[a-zA-Z0-9_\-]+)?$ ]]; then
    echo "Unable to extract version from $1 $version"
    exit 1
  fi
  echo "$version"
}

check_command "dch" "debhelper"
check_command "lsb_release" "lsb-release"

pecl_name="cassandra"
pecl_version=$(header_version "../version.h")
version=$(header_version_full "../version.h")
release=1
dist=$(lsb_release -s -c)
base="php-driver-$version"
files="config.m4 php_driver.c php_driver.h php_driver_globals.h php_driver_types.h version.h src util"

echo "Building version $version"

libuv_version=$(dpkg -s libuv | grep 'Version' | awk '{ print $2 }')

if [[ -e $libuv_version ]]; then
  echo "'libuv' required, but not installed"
  exit 1
fi

echo "Using libuv version $libuv_version"

if [[ -d build ]]; then
  read -p "Build directory exists, remove? [y|n] " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    rm -rf build
  fi
fi
mkdir -p "build/$base"

echo "Copying files"
mkdir -p "build/$base/$pecl_name-$pecl_version"
cp ../package.xml "build/$base"
for file in $files; do
  cp -r  "../$file" "build/$base/$pecl_name-$pecl_version/$file"
done
cp -r debian "build/$base"

pushd "build/$base"
echo "Updating changlog"
dch -m -v "$version-$release" -D $dist "Version $version"

echo "Building package:"
nprocs=$(grep -e '^processor' -c /proc/cpuinfo)
DH_PHP_VERSIONS_OVERRIDE=7.0 DEB_BUILD_OPTIONS="parallel=$nprocs" debuild -i -b -uc -us
popd
