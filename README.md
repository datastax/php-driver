# DataStax PHP Driver for Apache Cassandra

This is a wrapper around the DataStax C/C++ driver for Apache Cassandra. This
driver works exclusively with the Cassandra Query Language version 3 (CQL3) and
Cassandra's native protocol.

* Code: [https://github.com/datastax/php-driver](https://github.com/datastax/php-driver)
* Jira: [https://datastax-oss.atlassian.net/browse/PHP](https://datastax-oss.atlassian.net/browse/PHP)
* Mailing List: [https://groups.google.com/a/lists.datastax.com/forum/#!forum/php-driver-user](https://groups.google.com/a/lists.datastax.com/forum/#!forum/php-driver-user)
* IRC: #datastax-drivers on [irc.freenode.net](http://freenode.net>)
* Twitter: Follow the latest news about DataStax Drivers - [@avalanche123](http://twitter.com/avalanche123), [@mfiguiere](http://twitter.com/mfiguiere), [@al3xandru](https://twitter.com/al3xandru)

## Status of v1.0.0.alpha

The current release is an early alpha. This means that it lacks some features
and stability, but should be enough to start testing and working with Apache
Cassandra. Below, you'll find a list of functionality that is already supported
as well as what's plan for the upcoming releases.

## Features

* [All pre 2.1 Cassandra datatypes, including sets, lists and maps](features/datatypes.feature).
* [Simple](features/simple_statements.feature), [prepared](features/prepared_statements.feature) and [batch statements](features/batch_statements.feature).
* [Results paging](feature/result_paging.feature).
* [Asynchronous IO](src/Cassandra/Session.php#L24-L35).
* [SSL encryption](feature/ssl_encryption.feature).
* [Credentials authentication](src/Cassandra/Cluster/Builder.php#L312-L320).

## Planned

* Windows support.
* Expose [Schema Metadata](http://datastax.github.io/cpp-driver/api/struct_cass_session/#1afc51badec695483e4cde43ae0fc1e876).
* Add arithmetic functions to numeric types.

## Quick Start

```php
<?php
$cluster   = Cassandra::cluster()                 // connects to localhost by default
                 ->build();
$keyspace  = 'system';
$session   = $cluster->connect($system);          // create session, optionally scoped to a keyspace
$statement = new Cassandra\SimpleStatement(       // also supports prepared and batch statements
  'SELECT keyspace_name, columnfamily_name FROM schema_columnfamilies'
);
$future    = $session->executeAsync($statement);  // fully asynchronous and easy parallel execution
$result    = $future->get();                      // wait for result to become available, optionally set a timeout

foreach ($result as $row) {                       // results and rows implement Iterator, Countable and ArrayAccess
    printf("The keyspace %s has a table called %s\n", $row['keyspace_name'], $row['columnfamily_name']);
}
```

## Installation

**NOTE**: You must first [install and enable the `cassandra.so` PHP extension](ext/README.md).

* First, [get composer](https://getcomposer.org/).
* Then, add the following to your `composer.json`:

```json
{
    "require": {
        "datastax/php-driver": "1.0.*@alpha"
    }
}
```

* Finally, install the DataStax PHP Driver for Apache Cassandra:

```bash
composer install
```

## Compatibility

This driver works exclusively with the Cassandra Query Language v3 (CQL3) and
Cassandra's native protocol. The current version works with:

* Apache Cassandra versions 1.2, 2.0 and partially 2.1
* DataStax Enterprise 3.1, 3.2, 4.0 and 4.5
* PHP 5.3+

__NOTE__: Apache Cassandra 2.1 support is limited to the Cassandra 2.0 API, e.g. no user-defined types.

__NOTE__: Windows support is planned for a later, more stable release.

## Contributing

[Read our contribution policy](CONTRIBUTING.md) for a detailed description of the process.

## Code examples

The DataStax PHP Driver uses the amazing [Behat Framework](http://docs.behat.org/)
for both end-to-end, or acceptance, testing and documentation. All of the features
supported by the driver have appropriate acceptance tests with easy-to-copy code
examples in the `features/` directory.

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

Once you've logged in to the vagrant vm, run:

```bash
source venv/bin/activate
pip install ccm
cd /usr/local/src/php-driver
./bin/behat
./bin/phpunit
```
## Copyright

Copyright 2015 DataStax, Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
