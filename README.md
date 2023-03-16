
# ScyllaDB/CassandraDB Driver for PHP

[![Build Status](https://github.com/he4rt/scylladb-php-driver/actions/workflows/tests.yml/badge.svg?branch=v1.3.x)](https://github.com/he4rt/scylladb-php-driver/actions/workflows/tests.yml)

A modern, [feature-rich][Features] and highly tunable PHP client library for [ScyllaDB](https://github.com/scylladb/scylladb) and
[Apache Cassandra] 3.0+ using exclusively Cassandra's binary protocol. 

This is a wrapper around the [ScyllaDB C/C++ Driver].


## Getting the Driver

Binary versions of the driver, available for Linux systems and officialy supported versions of PHP (8.1 and 8.2), can be obtained from GitHub Releases (soon).

You're also can compile the driver by yourself or use Dockerfile with a pre-set environment to run your tests.


## What's new in v1.2.0/v1.3.8

* ScyllaDB C/C++ Shard Aware driver implemented
* Support for [`duration`]
* `Session::execute()` and `Session::executeAsync()` now support a
  [simple string] for the query CQL and a simple array for the query execution option
* Full support for Apache Cassandra  3.0+
* Support for [`tinyint` and `smallint`]
* Support for [`date`] and [`time`]
* Support for [user-defined function and aggregate] metadata
* Support for [secondary index] and [materialized view] metadata

## Last Development Status
### v1.3.8
- Migration from C to C++
- Removing PHP Build system in favor of CMake
- Upgraded `Cassandra\Cluster\Builder` class to new PHP argument parsing API
- Reduce memory usage from `Cassandra\Cluster\Builder`
- Migrate from Behat to PestPHP
- Migrated from TravisCI to Github Actions.

## Compatibility

This driver works exclusively with the Cassandra Query Language v3 (CQL3) and
Cassandra's native protocol. The current version works with:

* ScyllaDB 4.4.x and 5.x +
* Apache Cassandra versions 3.0+
* PHP 8.1 and 8.2 
* 64-bit (x64)
* Thread safe (TS) and non-thread safe (NTS)
* Compilers: GCC 10.0+ and Clang 14+


<!-- ## Documentation

* [Home]
* [API]
* [Features] -->

## Getting Help

* If you're able to fix a bug yourself, you can [fork the repository](https://help.github.com/articles/fork-a-repo/) and [submit a pull request](https://help.github.com/articles/using-pull-requests/) with the fix.
* If you're not able fix a bug yourself, please [open an issue](https://github.com/he4rt/scylladb-php-driver/issues) , describe it with the most details possible and wait until one of our maintainers join the conversation. 

## Quick Start

```php
<?php
$cluster   = Cassandra::cluster()                 // connects to localhost by default
                 ->build();
$keyspace  = 'system';
$session   = $cluster->connect($keyspace);        // create session, optionally scoped to a keyspace
$statement = new Cassandra\SimpleStatement(       // also supports prepared and batch statements
    'SELECT keyspace_name, columnfamily_name FROM schema_columnfamilies'
);
$querySent = $session->execute($statement);  
$result    = $querySent->get();                      // wait for the result, with an optional timeout

foreach ($result as $row) {                       // results and rows implement Iterator, Countable and ArrayAccess
    printf("The keyspace %s has a table called %s\n", $row['keyspace_name'], $row['columnfamily_name']);
}
```

## Installation

Today we have support to build the release version 

### Compiling Release Build

need the php-dev to build it (right version for your environment)

```shell
apt update -y 
apt upgrade -y 
apt install -y python3 python3-pip unzip mlocate build-essential ninja-build libssl-dev libgmp-dev zlib1g-dev openssl libpcre3-dev php-dev && pip3 install cmake

cmake --preset Release  && cd out/Release && sudo ninja install
```

> ninja install needs to be running at sudo

move the cassandra.ini into your php environment

### Compiling Development Build

```shell

apt update -y && apt upgrade -y && apt install -y python3 python3-pip unzip mlocate build-essential ninja-build libssl-dev libgmp-dev zlib1g-dev openssl libpcre3-dev && pip3 install cmake cqlsh && install-php-extensions intl zip pcntl gmp composer && apt-get clean

cmake --preset Debug && cd out/Debug && ninja

php -d "extension=$(pwd)/out/Debug/cassandra.so" debug.php
```

### Compiling XTREME optimization (pls be careful so powerful)

need the php-dev to build it (right version for your environment)
perfect for your cpu < 

```shell
apt update -y 
apt upgrade -y 
apt install -y python3 python3-pip unzip mlocate build-essential ninja-build libssl-dev libgmp-dev zlib1g-dev openssl libpcre3-dev php-dev && pip3 install cmake

cmake --preset Release -DPHP_SCYLLADB_OPTIMISE_FOR_CURRENT_MACHINE=ON && cd out/Release && sudo ninja install
```

## Contributing

[Read our contribution policy][contribution-policy] for a detailed description
of the process.

## Running tests

For your convenience a `Vagrantfile` with configuration ready for testing is
available. To execute tests, run the following:

```bash
git clone https://github.com/datastax/php-driver.git
cd php-driver
git submodule update --init
vagrant up
vagrant ssh
```

Once you've logged in to the vagrant VM, run:

```bash
cd /usr/local/src/php-driver
./bin/behat
./bin/phpunit
```

## Copyright

&copy; DataStax, Inc.

Licensed under the Apache License, Version 2.0 (the “License”); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

[Apache Cassandra]: http://cassandra.apache.org
[DSE PHP driver]: http://docs.datastax.com/en/developer/php-driver-dse/latest
[DataStax Enterprise]: http://www.datastax.com/products/datastax-enterprise
[DataStax C/C++ Driver for Apache Cassandra]: http://docs.datastax.com/en/developer/cpp-driver/latest
[DataStax download server]: http://downloads.datastax.com/php-driver
[GitHub]: https://github.com/datastax/php-driver
[Home]: http://docs.datastax.com/en/developer/php-driver/latest
[API]: http://docs.datastax.com/en/developer/php-driver/latest/api
[installing-details]: https://github.com/datastax/php-driver/blob/master/ext/README.md
[contribution-policy]: https://github.com/datastax/php-driver/blob/master/CONTRIBUTING.md
[Behat Framework]: http://docs.behat.org
[Features]: /tests/features
[`duration`]: /tests/features/duration.feature
[simple string]: /tests/features/simple_string_queries.feature
[`tinyint` and `smallint`]: /tests/features/datatypes.feature#L92
[`date`]: /tests/features/datatypes.feature#L135
[`time`]: /tests/features/datatypes.feature#L170
[user-defined function and aggregate]: /tests/features/function_and_aggregate_metadata.feature
[secondary index]: /tests/features/secondary_index_metadata.feature
[materialized view]: /tests/features/materialized_view_metadata.feature
