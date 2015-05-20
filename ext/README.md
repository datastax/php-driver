# Cassandra PHP Extension

The Cassandra PHP Extension depends on the following libraries:

* [The C/C++ driver and its dependencies](http://datastax.github.io/cpp-driver/topics/#installation).
* [The GNU Multiple Precision Arithmetic Library](https://gmplib.org/).
* [Libuv](http://libuv.org/)

You can build the extension yourself or use one of the provided scripts.

## Install dependencies

### Linux/OS X

* homebrew

```bash
brew install libuv cmake gmp
```

* apt-get

```bash
sudo apt-get install g++ make cmake libuv-dev libssl-dev libgmp-dev php5 php5-dev openssl libpcre3-dev
```

* yum

```bash
sudo yum install automake cmake gcc gcc-c++ git libtool openssl-devel wget gmp gmp-devel boost php-devel pcre-devel
pushd /tmp
wget http://libuv.org/dist/v1.4.2/libuv-v1.4.2.tar.gz
tar xzf libuv-v1.4.2.tar.gz
pushd libuv-v1.4.2
sh autogen.sh
./configure
sudo make install
popd
popd
```

[Refer to the official documentation](http://datastax.github.io/cpp-driver/topics/building/)
for the DataStax C/C++ Driver for Apache Cassandra in case you're having issues
installing any of its dependencies.

## Install the PHP extension

### Installing with pecl

The PHP driver is not published to the official PECL repository yes. You can
still install it using pecl by specifying the provided `package.xml` file path
as the argument to `pecl install` command.

[Install the 2.0 version of the C/C++ driver](http://datastax.github.io/cpp-driver/topics/building/)

```bash
git clone https://github.com/datastax/php-driver.git
cd php-driver
pecl install ext/package.xml
```

### Installing with submoduled, statically compiled version of the C/C++ driver

```bash
git clone https://github.com/datastax/php-driver.git
cd php-driver
git submodule update --init
cd ext
./install.sh
```

**Note** [The install.sh script](install.sh#L25-L35) will also compile and
statically link into the extension a submoduled version of the DataStax C/C++
driver for Apache Cassandra. To use a version of cpp driver that you already
have on your system, run `phpize`, `./configure` and `make install`.

#### Enable the PHP extension

```bash
echo "# DataStax PHP Driver\nextension=cassandra.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
```

### Installing on Windows

The library dependencies will automatically download and build; however the
following build dependencies will need to be installed.

#### Obtaining Build Dependencies

- Download and install [Bison](http://gnuwin32.sourceforge.net/downlinks/bison.php).
 - Make sure Bison is in your system PATH and not installed in a directory with
   spaces (e.g. `%SYSTEMDRIVE%\GnuWin32`)
- Download and install [CMake](http://www.cmake.org/download).
 - Make sure to select the option "Add CMake to the system PATH for all users"
   or "Add CMake to the system PATH for current user".
- Download and install [Git](http://git-scm.com/download/win)
 - Make sure to select the option "Use Git from Windows Command Prompt" or
   manually add the git executable to the system PATH.
- Download and install [ActiveState Perl](https://www.perl.org/get.html#win32)
 - Make sure to select the option "Add Perl to PATH environment variable".
- Download and install [Python v2.7.x](https://www.python.org/downloads)
 - Make sure to select/install the feature "Add python.exe to Path"

#### Building the Driver

A batch script has been created to detect installed versions of Visual Studio
to simplify the build process on Windows. If you have more than one version of
Visual Studio installed you will be prompted to which version to use when
compiling the driver.

First you will need to open a "Command Prompt" to execute the batch script.

```dos
Usage: VC_BUILD.BAT [OPTION...]

    --DEBUG                           Enable debug build
    --RELEASE                         Enable release build (default)
    --DISABLE-CLEAN                   Disable clean build
    --DISABLE-THREAD-SAFETY           Disable thread safety
    --ENABLE-TEST-CONFIGURATION       Enable test configuration build
    --PHP-VERSION [version]           PHP version 5.3, 5.4, 5.5, or 5.6 (*)
    --X86                             Target 32-bit build (**)
    --X64                             Target 64-bit build (**)

    C/C++ Driver Options
      --USE-BOOST-ATOMIC              Use Boost atomic

    --HELP                            Display this message

*  Defaults to PHP v5.6 if --PHP-VERSION is not used
** Default target architecture is determined based on system architecture
```

To build 32-bit extension library with Zend thread safety:

```dos
VC_BUILD.BAT --X86
```

To build 32-bit extension library with thread safety disabled:

```dos
VC_BUILD.BAT --X86 --DISABLE-THREAD-SAFETY
```

To build 64-bit shared library with Zend thread safety:

```dos
VC_BUILD.BAT --X64
```

To build the default system architecture using PHP v5.5:

```dos
VC_BUILD.BAT --PHP-VERSION 5.5
```

To build the default system architecture using Boost atomic implementation:

```dos
VC_BUILD.BAT --USE-BOOST-ATOMIC
```

#### Testing

Ensure the driver is built with --ENABLE-TEST-CONFIGURATION in order to execute
the [Behat](http://www.behat.org) test suite and PHPUnit unit tests. You will
also need to install CCM; detailed instructions can be found in this blog
[post](http://www.datastax.com/dev/blog/ccm-2-0-and-windows).
