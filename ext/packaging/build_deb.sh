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
version=$(header_version_full "../version.h")
release=1
dist=$(lsb_release -s -c)
base="php-driver-$version"
files="config.m4 php_driver.c php_driver.h php_driver_globals.h php_driver_types.h version.h src util"


if [[ -z $1 ]]; then
  php_version=$(php-config --version | sed -rn 's/^([0-9]+)\.([0-9]+)(.*)/\1.\2/p')
else
  php_version=$1
fi

debian_dir="debian-php$php_version"

if [[ ! -e $debian_dir ]]; then
  echo "PHP $php_version not supported"
  exit -1
fi

echo "Building version $version with PHP $php_version"

if [[ -d build ]]; then
  read -p "Build directory exists, remove? [y|n] " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    rm -rf build
  fi
fi
mkdir -p "build/$base"

echo "Copying files"
cp ../package.xml "build/$base"
for file in $files; do
  cp -r  "../$file" "build/$base/$file"
done

cp -r $debian_dir "build/$base/debian"

pushd "build/$base"
echo "Updating changlog"
dch -M --create --package "php$php_version-cassandra-driver" -v "$version-$release" -D $dist "Version $version"
echo "Building package:"
nprocs=$(grep -e '^processor' -c /proc/cpuinfo)
DEB_BUILD_OPTIONS="parallel=$nprocs" debuild -e PHP_VERSION="$php_version" -i -b -uc -us 

popd
