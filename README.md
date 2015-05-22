# DataStax PHP Driver for Apache Cassandra

[![Build Status](https://travis-ci.org/datastax/php-driver.svg)](https://travis-ci.org/datastax/php-driver)

A modern, [feature-rich](features) and highly tunable PHP client library for [Apache Cassandra](http://cassandra.apache.org/) (1.2+) and [DataStax Enterprise](http://www.datastax.com/products/products-index) (3.1+) using exclusively Cassandra's binary protocol and Cassandra Query Language v3.

This is a wrapper around [the DataStax C/C++ Driver for Apache Cassandra and DataStax Enterprise](http://datastax.github.io/cpp-driver/).

* Docs: [http://datastax.github.io/php-driver/](http://datastax.github.io/php-driver/)
* Code: [https://github.com/datastax/php-driver](https://github.com/datastax/php-driver)
* Jira: [https://datastax-oss.atlassian.net/browse/PHP](https://datastax-oss.atlassian.net/browse/PHP)
* Mailing List: [https://groups.google.com/a/lists.datastax.com/forum/#!forum/php-driver-user](https://groups.google.com/a/lists.datastax.com/forum/#!forum/php-driver-user)
* IRC: #datastax-drivers on [irc.freenode.net](http://freenode.net>)
* Twitter: Follow the latest news about DataStax Drivers - [@avalanche123](http://twitter.com/avalanche123), [@mfiguiere](http://twitter.com/mfiguiere), [@al3xandru](https://twitter.com/al3xandru)

## Status of v1.0.0.beta

This is the first beta release of the DataStax PHP Driver. This means that it
is mostly feature complete but not entirely stable. Below, you'll find a list
of functionality that is already supported as well as what's planned for the
upcoming releases.

## What's new in v1.0.0.beta:

* [Persistent sessions](http://datastax.github.io/php-driver/features/#persistent-sessions).
* Windows support.
* Configurable logger (file/syslog and level).
* Math functions for numeric types and [`Cassandra\Numeric`](http://datastax.github.io/php-driver/api/interface/Cassandra/Numeric/) interface.
* All classes moved into the extension.
* Upgraded to use the C/C++ driver v2.0.
* Added stub files for IDEs.

## Planned

* Ability to configure connection pooling.
* Ability to configure latency aware load balancing.
* Expose [Schema Metadata](http://datastax.github.io/cpp-driver/api/struct_cass_session/#1afc51badec695483e4cde43ae0fc1e876).

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
$future    = $session->executeAsync($statement);  // fully asynchronous and easy parallel execution
$result    = $future->get();                      // wait for the result, with an optional timeout

foreach ($result as $row) {                       // results and rows implement Iterator, Countable and ArrayAccess
    printf("The keyspace %s has a table called %s\n", $row['keyspace_name'], $row['columnfamily_name']);
}
```

## Installation

[Read detailed instructions on building and installing the extension](https://github.com/datastax/php-driver/blob/master/ext/README.md)

## Compatibility

This driver works exclusively with the Cassandra Query Language v3 (CQL3) and
Cassandra's native protocol. The current version works with:

* Apache Cassandra versions 1.2, 2.0 and partially 2.1
* DataStax Enterprise 3.1, 3.2, 4.0 and 4.5
* PHP 5.3+
* Compilers: GCC 4.1.2+, Clang 3.4+, and MSVC 2010/2012/2013

__NOTE__: Apache Cassandra 2.1 support is limited to the Cassandra 2.0 API (native protocol v2), e.g. no user-defined types.

## Contributing

[Read our contribution policy](https://github.com/datastax/php-driver/blob/master/CONTRIBUTING.md) for a detailed description of the process.

## Code examples

The DataStax PHP Driver uses the amazing [Behat Framework](http://docs.behat.org/)
for both end-to-end, or acceptance, testing and documentation. All of the features
supported by the driver have appropriate acceptance tests with [easy-to-copy code
examples in the `features/` directory](https://github.com/datastax/php-driver/tree/master/features).

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

Copyright 2015 DataStax, Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
