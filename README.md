# PHP Cassandra - A PHP Driver for Apache Cassandra

This is a wrapper around the DataStax C/C++ driver.

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

* [Get composer](https://getcomposer.org/).
* Add the following to your `composer.json`:

```json
{
    "require": {
        "datastax/php-driver": "~@1.0"
    }
}
```

* Install DataStax PHP Driver for Apache Cassandra:

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

## What's new in v1.0.0.alpha

Features:

* [All pre 2.1 Cassandra datatypes, including sets, lists and maps](features/datatypes.feature).
* [Simple](features/simple_statements.feature), [prepared](features/prepared_statements.feature) and [batch statements](features/batch_statements.feature).
* [Results paging](feature/result_paging.feature).
* [SSL encryption](feature/ssl_encryption.feature).
* [Credentials authentication](src/Cassandra/Cluster/Builder.php#L312-L320).
