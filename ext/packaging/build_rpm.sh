#!/bin/bash

function check_command {
  command=$1
  package=$2
  if ! type "$command" > /dev/null 2>&1; then
    echo "Missing command '$command', run: yum install $package"
    exit 1
  fi
}

function header_version {
  read -d ''  version_script << 'EOF'
  BEGIN { major="?"; minor="?"; patch="?" }
  /PHP_DRIVER_MAJOR/ { major=$3 }
  /PHP_DRIVER_MINOR/ { minor=$3 }
  /PHP_DRIVER_RELEASE/ { patch=$3 }
  /PHP_DRIVER_STABILITY/ { suffix=$3; gsub(/"/, "", suffix) }
  END {
    if (length(suffix) > 0)
      printf "%s.%s.%s%s", major, minor, patch, suffix
    else
      printf "%s.%s.%s", major, minor, patch
  }
EOF
  version=$(grep '#define[ \t]\+PHP_DRIVER_\(MAJOR\|MINOR\|RELEASE\|STABILITY\)' $1 | awk "$version_script")
  if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+([a-zA-Z0-9_\-]+)?$ ]]; then
    echo "Unable to extract version from $1"
    exit 1
  fi
  echo "$version"
}

# 'redhat-rpm-config' needs to be installed for the 'debuginfo' package
check_command "rpmbuild" "rpm-build"

arch="x86_64"
if [[ ! -z $1 ]]; then
  arch=$1
fi

echo "Getting PHP version"
php_version=$(php-config --version | sed -rn 's/^([0-9]+)\.([0-9]+)\.[0-9]+/\1\2w/p')

version=$(header_version "../version.h")
package_name="php$php_version-cassandra-driver"
base="$package_name-$version"
archive="$base.tar.gz"
files="config.m4 php_driver.c php_driver.h php_driver_globals.h php_driver_types.h version.h src util"

echo "Building version $version"

libuv_version=$(rpm -q --queryformat "%{VERSION}" libuv)

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
mkdir -p build/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
mkdir -p "build/SOURCES/$base"

echo "Copying files"
for file in $files; do
  cp -r  "../$file" "build/SOURCES/$base/$file"
done

echo "Archiving $archive"
pushd "build/SOURCES"
tar zcf $archive $base
popd

echo "Building package:"
rpmbuild --target $arch --define "_topdir ${PWD}/build" --define "driver_version $version" --define "libuv_version $libuv_version" --define "package_name $package_name" -ba php-cassandra-driver.spec

exit 0
