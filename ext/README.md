# Cassandra PHP Extension

This extension is a wrapped around the [DataStax C/C++ driver for Apache
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
sudo apt-get install g++ make cmake libuv-dev libssl-dev libgmp-dev php5 php5-dev openssl
```

* yum

```bash
sudo yum install automake cmake gcc-c++ git libtool openssl-devel wget
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

## Install the PHP extension

```bash
git clone https://github.com/datastax/php-driver.git
cd php-driver
git submodule update --init
cd ext
./install.sh
```

**Note** In the future, a PECL installation will be provided.

## Enable the PHP extension

```bash
echo "# DataStax PHP Driver\nextension=cassandra.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
```
