# Building PHP Extension

The PHP driver is implemented as a native PHP extension and leverages the
[C/C++ driver][cpp-driver-docs]. The driver will build on most standard
Unix-like and Microsoft Windows platforms.

Packages are available for the following platforms:

* [CentOS 6][php-driver-centos6]
* [CentOS 7][php-driver-centos7]
* [Ubuntu 14.04 LTS][php-driver-ubuntu14-04]
* [Ubuntu 16.04 LTS][php-driver-ubuntu16-04]
* [Windows][php-driver-windows]

__NOTE__: The build procedures only need to be performed for driver development
          or if your system doesn't have packages available for download and
          installation.

## Compatibility

* PHP 5.6, PHP 7.0, and PHP 7.1
  * 32-bit (x86) and 64-bit (x64)
  * Thread safe (TS) and non-thread safe (NTS)
* Compilers: GCC 4.1.2+, Clang 3.4+, and MSVC 2012/2015

## Dependencies

The PHP Extension depends on the following software:

* [The C/C++ driver][cpp-driver-docs-build]
  * [CMake] v2.6.4+
  * [libuv] 1.x
  * [OpenSSL] v1.0.x or v1.1.x
* [The GNU Multiple Precision Arithmetic Library][gmp]

## Linux/Mac OS

The driver is known to build on CentOS/RHEL 6/7, Mac OS X 10.10/10.11 (Yosemite
and El Capitan), Mac OS 10.12 (Sierra), and Ubuntu 14.04/16.04 LTS.

__NOTE__: The driver will also build on most standard Unix-like systems using
          GCC 4.1.2+ or Clang 3.4+.

### Installing dependencies

#### Initial environment setup

##### CentOS/RHEL 6 (Yum)

CentOS/RHEL 6 ships with PHP v5.3.x as the default version of PHP; which is not
supported. A supported version of PHP can be installed by performing the
following:

```bash
yum install epel-release
rpm -Uvh https://mirror.webtatic.com/yum/el6/latest.rpm
```

Once completed PHP v5.6.x, v7.0.x, or v7.1.x can be installed:

```bash
yum install automake cmake gcc-c++ git libtool pcre-devel php71w-devel php71w-pear
```

##### CentOS/RHEL 7 (Yum)

CentOS/RHEL 7 ships with PHP v5.4.x as the default version of PHP; which is not
supported. A supported version of PHP can be installed by performing the
following:

```bash
yum install epel-release
rpm -Uvh https://mirror.webtatic.com/yum/el7/latest.rpm
```

Once completed PHP v5.6.x, v7.0.x, or v7.1.x can be installed:

```bash
yum install automake cmake gcc-c++ git libtool pcre-devel php71w-devel php71w-pear
```

##### Ubuntu 14.04 (APT)

Ubuntu 14.04 LTS (Debian 8) ships with PHP v5.5.x as the default version of
PHP; which is not supported. A supported version of PHP can be installed by
performing the following:

```bash
add-apt-repository ppa:ondrej/php
apt-get update
```

Once completed PHP v5.6.x, v7.0.x, or v7.1.x can be installed:

```bash
apt-get install build-essential cmake git libpcre3-dev php7.1-dev
```

##### Ubuntu 16.04 (APT)

Starting with Ubuntu 16.04 LTS (Debian 9) PHP v5.5.x has been removed as the
default version of PHP and has been replaced with either PHP v7.0.x or PHP
v7.1.x:

```bash
apt-get install build-essential cmake git libpcre3-dev php7.1-dev
```

##### Mac OS (Brew)

```bash
brew update
brew upgrade
brew tap homebrew/homebrew-php
brew install autoconf automake libtool php71 --with-pear
```

#### GNU Multiple Precision Arithmetic

##### CentOS/RHEL (Yum)

```bash
yum install gmp-devel
```

##### Ubuntu (APT)

```bash
apt-get install libgmp-dev
```

##### Mac OS (Brew)

```bash
brew install gmp
```

#### libuv

libuv v1.x should be used in order to ensure all features of the C/C++ driver
and the PHP driver are available. When using a package manager for your
operating system make sure you install v1.x; if available.

##### CentOS/RHEL and Ubuntu packages

Packages are available from our [download server]:

* [CentOS 6][libuv-centos6]
* [CentOS 7][libuv-centos7]
* [Ubuntu 14.04 LTS][libuv-ubuntu14-04]
* [Ubuntu 16.04 LTS][libuv-ubuntu16-04]

##### Mac OS (Brew)

```bash
brew install libuv
```

##### Manually build and install

_The following procedures should be performed if packages are not available for
your system._

```bash
pushd /tmp
wget http://dist.libuv.org/dist/v1.13.1/libuv-v1.13.1.tar.gz
tar xzf libuv-v1.13.1.tar.gz
pushd libuv-v1.13.1
sh autogen.sh
./configure
make install
popd
popd
```

#### OpenSSL

##### CentOS (Yum)

```bash
yum install openssl-devel
```

##### Ubuntu (APT)

```bash
apt-get install libssl-dev
```

##### Mac OS (Brew)

```bash
brew install openssl
```

__NOTE__: For Mac OS X 10.11 (El Capitan) and Mac OS 10.12 (Sierra) a link
          needs to be created in order to make OpenSSL available to the
          building libraries:

```bash
brew link --force openssl
```

#### C/C++ driver

##### CentOS/RHEL and Ubuntu packages

Packages are available from our [download server] and the latest version of the
driver be used:

* [CentOS 6][cpp-driver-centos6]
* [CentOS 7][cpp-driver-centos7]
* [Ubuntu 14.04 LTS][cpp-driver-ubuntu14-04]
* [Ubuntu 16.04 LTS][cpp-driver-ubuntu16-04]

##### Manually build and install

_The following procedures should be performed if packages are not available for
your system._

The C/C++ driver is made available as a submodule; if preferred the source is
also available [here][cpp-driver-source].

###### Updating source tree to build submodule

```bash
git update --init lib\cpp-driver
```

##### Building and installing the C/C++ driver

```bash
mkdir build
pushd build
cmake ..
make
make install
popd
```

[Refer to the official build documentation][cpp-driver-docs-build] for more
detailed instructions.

### Building and Installing the PHP extension

#### PECL

The [PHP Extension Community Library (PECL)][pecl] is a repository for PHP
extensions. A database [package][pecl-cassandra] which can ease building and
installing the PHP extension can be installed with the following:

```bash
pecl install cassandra
```

#### Manually build and install

_The following procedures should be performed if packages or PECL is not
available for your system._

```bash
pushd ext
phpize
popd
mkdir build
pushd build
../ext/configure
make
make install
popd
```

### Enabling the PHP extension

To determine where the `php.ini` file is located on your system run the
following command:

```bash
php -r "echo php_ini_loaded_file();"
```

Edit the `php.ini` file and add the following line to enable the extension:

```
; DataStax PHP Driver for Apache Cassandra
extension=cassandra.so
```

To verify the extension is being loaded the following command can be executed:

```bash
php -i | grep -A 10 "^cassandra$"
```

### Enabling testing framework

In order to execute the [Behat] test suite and [PHPUnit] unit and integration
tests [Composer] must downloaded and installed:

```bash
curl -sS https://getcomposer.org/installer | php
php composer.phar install
```

## Windows

We provide a self-contained [batch script](vc_build.bat) for building the PHP
driver and all of its dependencies. In order to run it, you have to install the
build dependencies and clone the repository with the DataStax PHP driver for
Apache Cassandra.

### Obtaining build dependencies

* Download and install [Bison]
  * Make sure Bison is in your system PATH and not installed in a directory with
    spaces (e.g. `%SYSTEMDRIVE%\GnuWin32`)
* Download and install [CMake]
  * Make sure to select the option "Add CMake to the system PATH for all users"
    or "Add CMake to the system PATH for current user"
* Download and install [Git]
  * Make sure to select the option "Use Git from Windows Command Prompt" or
    manually add the git executable to the system PATH.
* Download and install [ActiveState Perl]
  * Make sure to select the option "Add Perl to PATH environment variable"
* Download and install [Python v2.7.x][python-27]
  * Make sure to select/install the feature "Add python.exe to Path"

### Building the driver

The [batch script] detects installed versions of Visual Studio to simplify the
build process on Windows and select the correct version of Visual Studio for
the PHP version the driver is being built for.

First you will need to open a "Command Prompt" to execute the batch script.
Running the batch script without any arguments will build the driver for PHP
v7.1 and for the current system architecture (e.g. x64).

To perform advanced build configuration, execute the batch script with the
`--HELP` argument to display the options available.

```dos
Usage: VC_BUILD.BAT [OPTION...]

    --DEBUG                           Enable debug build
    --RELEASE                         Enable release build (default)
    --DISABLE-CLEAN                   Disable clean build
    --ENABLE-THREAD-SAFETY            Enable thread safety
    --ENABLE-PACKAGES [version]       Enable package generation (5.6, 7.0, 7.1) (*)
    --ENABLE-TEST-CONFIGURATION       Enable test configuration build
    --PHP-VERSION [version]           PHP version 5.6, 7.0, and 7.1 (**)
    --X86                             Target 32-bit build (***)
    --X64                             Target 64-bit build (***)

    C/C++ Driver Options
      --USE-BOOST-ATOMIC              Use Boost atomic

    --HELP                            Display this message

*   Minimum supported officially released PHP binary installations
**  Defaults to PHP v7.1 if --PHP-VERSION is not used
*** Default target architecture is determined based on system architecture
```
#### Enable testing

Ensure the driver is built with --ENABLE-TEST-CONFIGURATION in order to execute
the [Behat] test suite and [PHPUnit] unit and integration tests.

#### Manual/PHP step-by-step windows build

The PHP driver extension can also be built using the
[Build your own PHP on Windows] instructions followed by the
[Building PECL extensions] instruction where the driver can be statically
linked into the PHP executable or as an import (DLL) library. This process
requires that the binary dependencies of the PHP driver extension be included
in the`phpdev\vc##\x##\deps` directory along with the standard PHP library
dependencies. Use `--enable-cassandra` to build library into the PHP executable
and `--enable-cassandra=shared` for import (DLL) library.

The PHP driver extension dependencies not included with the standard PHP
library can be download [here][cpp-driver-windows].

__Note__: The binary libraries downloaded/used must be compatible with the
          MSVC compiler and the PHP driver extension.

# Testing PHP Extension

## Features

### Linux/Mac OS

```bash
bin/behat
```

### Windows
```dos
bin\behat.bat
```

## Unit and integration tests

### Linux/Mac OS

```bash
bin/phpunit
```

### Windows
```dos
bin\phpunit.bat
```

[download server]: http://downloads.datastax.com
[php-driver-centos6]: http://downloads.datastax.com/php-driver/centos/6/cassandra
[php-driver-centos7]: http://downloads.datastax.com/php-driver/centos/7/cassandra
[php-driver-ubuntu14-04]: http://downloads.datastax.com/php-driver/ubuntu/14.04/cassandra
[php-driver-ubuntu16-04]: http://downloads.datastax.com/php-driver/ubuntu/16.04/cassandra
[php-driver-windows]: http://downloads.datastax.com/php-driver/windows/cassandra
[libuv-centos6]: http://downloads.datastax.com/cpp-driver/centos/6/dependencies/libuv
[libuv-centos7]: http://downloads.datastax.com/cpp-driver/centos/7/dependencies/libuv
[libuv-ubuntu14-04]: http://downloads.datastax.com/cpp-driver/ubuntu/14.04/dependencies/libuv
[libuv-ubuntu16-04]: http://downloads.datastax.com/cpp-driver/ubuntu/16.04/dependencies/libuv
[cpp-driver-centos6]: http://downloads.datastax.com/cpp-driver/centos/6/cassandra
[cpp-driver-centos7]: http://downloads.datastax.com/cpp-driver/centos/7/cassandra
[cpp-driver-ubuntu14-04]: http://downloads.datastax.com/cpp-driver/ubuntu/14.04/cassandra
[cpp-driver-ubuntu16-04]: http://downloads.datastax.com/cpp-driver/ubuntu/16.04/cassandra
[cpp-driver-windows]: http://downloads.datastax.com/cpp-driver/windows/cassandra
[cpp-driver-docs]: http://docs.datastax.com/en/developer/cpp-driver/latest
[cpp-driver-docs-build]: http://docs.datastax.com/en/developer/cpp-driver/latest/topics/building
[cpp-driver-source]: https://github.com/datastax/cpp-driver
[batch script]: vc_build.bat
[pecl]: https://pecl.php.net
[pecl-cassandra]: https://pecl.php.net/package/cassandra
[Bison]: http://gnuwin32.sourceforge.net/downlinks/bison.php
[CMake]: http://www.cmake.org/download
[Git]: http://git-scm.com/download/win
[ActiveState Perl]: https://www.perl.org/get.html#win32
[python-27]: https://www.python.org/downloads
[libuv]: http://libuv.org
[OpenSSL]: https://www.openssl.org
[gmp]: https://gmplib.org
[Build your own PHP on Windows]: https://wiki.php.net/internals/windows/stepbystepbuild
[Building PECL extensions]: https://wiki.php.net/internals/windows/stepbystepbuild#building_pecl_extensions
[Behat]: http://behat.org
[PHPUnit]: https://phpunit.de
[Composer]: https://getcomposer.org
