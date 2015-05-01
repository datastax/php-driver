# Cassandra PHP Extension

This extension is a wrapper around the [DataStax C/C++ driver for Apache
Cassandra](http://datastax.github.io/cpp-driver/). In order to install it, you
must install the C/C++ driver and its dependencies, as well as the [The GNU
Multiple Precision Arithmetic Library](https://gmplib.org/).

## Install dependencies

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
wget http://libuv.org/dist/v1.4.0/libuv-v1.4.0.tar.gz
tar xzf libuv-v1.4.0.tar.gz
pushd libuv-v1.4.0
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

```bash
git clone https://github.com/datastax/php-driver.git
cd php-driver
git submodule update --init
cd ext
./install.sh
```

**Note** In the future, a PECL installation will be provided.

**Note** [The install.sh script](install.sh#L25-L35) will also compile and
statically link into the extension a submoduled version of the DataStax C/C++
driver for Apache Cassandra. To use a version of cpp driver that you already
have on your system, run `phpize`, `./configure` and `make install`.

## Enable the PHP extension

```bash
echo "# DataStax PHP Driver\nextension=cassandra.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
```
