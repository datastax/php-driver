
# ScyllaDB/CassandraDB Driver for PHP

[![Build Status: Linux](https://travis-ci.org/datastax/php-driver.svg)](https://travis-ci.org/datastax/php-driver)
[![Build Status: Windows](https://ci.appveyor.com/api/projects/status/8vrxpkfl4xm2f3nm?svg=true)](https://ci.appveyor.com/project/DataStax/php-driver)

A modern, [feature-rich][Features] and highly tunable PHP client library for
[Apache Cassandra] 3.0+ using exclusively Cassandra's binary protocol. 

This is a wrapper around the [DataStax C/C++ Driver for Apache Cassandra].

__Note__: DataStax products do not support big-endian systems.

## Getting the Driver

Binary versions of the driver, available for multiple operating systems and
multiple versions of PHP, can be obtained from [DataStax download server]. 
You're also can compile the driver by yourself or use Dockerfile with a pre-set environment to run your tests.


## What's new in v1.2.0/v1.3.8

* Support for [`duration`]
* `Session::execute()` and `Session::executeAsync()` now support a
  [simple string] for the query CQL and a simple array for the query execution
  option
* Full support for Apache Cassandra  3.0+
* Support for [`tinyint` and `smallint`]
* Support for [`date`] and [`time`]
* Support for [user-defined function and aggregate] metadata
* Support for [secondary index] and [materialized view] metadata

## Compatibility

This driver works exclusively with the Cassandra Query Language v3 (CQL3) and
Cassandra's native protocol. The current version works with:

* Apache Cassandra versions 3.0+
* PHP 8.0, 8.1 and 8.2 
* 64-bit (x64)
* Thread safe (TS) and non-thread safe (NTS)
* Compilers: GCC 10.0+ and Clang 14+


## Documentation

* [Home]
* [API]
* [Features]

## Getting Help

* If you're able to fix a bug yourself, you can [fork the repository](https://help.github.com/articles/fork-a-repo/) and [submit a pull request](https://help.github.com/articles/using-pull-requests/) with the fix.
* If you're not able fix a bug yourself, please [open an issue](https://github.com/nano-interactive/ext-cassandra/issues) , describe it with the most details possible and wait until one of our maintainers join the conversation. 

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


[Read detailed instructions on building and installing the
extension][installing-details]



Or if you want to use a older version (PHP 7.1) use the command below
```bash
pecl install cassandra
```


## Contributing

[Read our contribution policy][contribution-policy] for a detailed description
of the process.

## Code examples

The DataStax PHP Driver uses the amazing [Behat Framework] for both end-to-end,
or acceptance testing and documentation. All of the features supported by the
driver have appropriate acceptance tests with [easy-to-copy code examples in
the `features/` directory][Features].

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
