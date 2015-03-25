# Cassandra PHP Extension

## Install dependencies

```bash
brew install libuv gmp
brew install https://raw.githubusercontent.com/datastax/cpp-driver/master/packaging/homebrew/cassandra-cpp-driver.rb
```

## Compile and Install the extension

```bash
cd php-driver/ext
phpize
./configure --with-cassandra
make
make test
sudo make install
```
