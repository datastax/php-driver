# Features

## Usage

### Specifying addresses of Cassandra nodes

[`withContactPoints()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-contact-points) and [`withPort()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-port) methods of the [`Cassandra\Cluster\Builder`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/) are used to specify ip addresses or hostnames and port number of the nodes in a given Cassandra cluster.

```php
<?php

$cluster = Cassandra::cluster()
               ->withContactPoints('10.0.1.24', 'example.com', 'localhost')
               ->withPort(9042)
               ->build();
$session = $cluster->connect();
```

### Discovering nodes in the cluster

After the initial connection to one of the hosts specified via `withContactPoints` succeeded, the driver discovers the addresses and connects to all members of the cluster automatically. You can also see the nodes that the driver discovered by running `SELECT * FROM system.peers`.

### Executing queries

You run CQL statements by passing them to [`Cassandra\Session::execute()`](http://datastax.github.io/php-driver/api/interface/Cassandra/Session/).

```php
<?php

$result = $session->execute(new Cassandra\SimpleStatement('SELECT keyspace_name, columnfamily_name FROM system.schema_columnfamilies'));

foreach ($result as $row) {
  sprintf("The keyspace \"%s\" has a table \"%s\".\n", $row['keyspace_name'], $row['columnfamily_name']);
}
```

### Parameterized queries

**If you're using Cassandra 2.0 or later** you no longer have to build CQL strings when you want to insert a value in a query, there's a new feature that lets you bind values with regular statements:

```php
<?php

$session->execute(
  new Cassandra\SimpleStatement("UPDATE users SET age = ? WHERE user_name = ?"),
  new Cassandra\ExecutionOptions(array(
    'arguments' => array(41, 'Sam')
  ))
);
```

If you find yourself doing this often, it's better to use prepared statements. As a rule of thumb, if your application is sending a request more than once, a prepared statement is almost always the right choice.

### Prepared Statements

The driver supports prepared statements. Use [`Cassandra\Session::prepare()`](http://datastax.github.io/php-driver/api/interface/Cassandra/Session/#prepare) to create a [`Cassandra\PreparedStatement` object](http://datastax.github.io/php-driver/api/class/Cassandra/PreparedStatement/), and then call [`Cassandra\Session::execute()`](http://datastax.github.io/php-driver/api/interface/Cassandra/Session/#execute).

```php
<?php

$statement = $session->prepare('INSERT INTO users (username, email) VALUES (?, ?)');

$session->execute($statement, new Cassandra\ExecutionOptions(array(
  'arguments' => array('avalanche123', 'bulat.shakirzyanov@datastax.com')
)));
```

A prepared statement can be run many times, but the CQL parsing will only be done once on each node. Use prepared statements for queries you run over and over again.

### Executing statements in parallel

With fully asynchronous api, it is very easy to run queries in parallel:

```php
<?php

$data = array(
  array(41, 'Sam'),
  array(35, 'Bob')
);

$statement = $session->prepare("UPDATE users SET age = ? WHERE user_name = ?");
$futures   = array();
$timeout   = 5;

// execute all statements in background
foreach ($data as $arguments) {
  $futures[]= $session->executeAsync($statement, new ExecutionOptions(
                'arguments' => $arguments
              ));
}

// wait for all statements to complete
foreach ($futures as $future) {
  // we will not wait for each result for more than 5 seconds
  $future->get(5);
}
```

Note that it is not enough to simply create a [`Cassandra\Future`](http://datastax.github.io/php-driver/api/interface/Cassandra/Future/) by calling one of the `*Async()` methods, you must ensure that this future has enough time to be executed by calling [`Cassandra\Future::get()`](http://datastax.github.io/php-driver/api/interface/Cassandra/Future/#get).

### Creating keyspaces and tables

There is no special facility for creating keyspaces and tables, they are created by executing CQL:

```php
<?php

$createKeyspace = new Cassandra\SimpleStatement(<<<EOD
CREATE KEYSPACE measurements
WITH replication = {
  'class': 'SimpleStrategy',
  'replication_factor': 1
}
EOD
);

$createTable = new Cassandra\SimpleStatement(<<<EOD
CREATE TABLE events (
  id INT,
  date DATE,
  comment VARCHAR,
  PRIMARY KEY (id)
)
EOD
);

$session->execute($createKeyspace);
$session->execute('USE measurements');
$session->execute($createTable);
```

You can also `ALTER` keyspaces and tables, and you can read more about that in the [CQL3 syntax documentation][1].

### Batch statements

**If you're using Cassandra 2.0 or later** you can build batch requests, either from simple or prepared statements. Batches must not contain any select statements, only `INSERT`, `UPDATE` and `DELETE` statements are allowed.

You can mix any combination of statements in a batch:

```php
<?php

$batch = new Cassandra\BatchStatement();

$statement = $session->prepare("UPDATE users SET name = ? WHERE user_id = ?");
$batch->add($statement, array('Sue', 'unicorn31'));

$statement = new Cassandra\SimpleStatement("UPDATE users SET age = 19 WHERE user_id = 'unicorn31'");
$batch->add($statement);

$statement = new Cassandra\SimpleStatement("INSERT INTO activity (user_id, what, when) VALUES (?, 'login', NOW())");
$batch->add($statement, array('unicorn31'));

$session->execute($batch);
```

Batches can have one of three different types: `logged`, `unlogged` or `counter`, where `logged` is the default. Their exact semantics are defined in the [Cassandra documentation][2], but this is how you specify which one you want:

[Read more about `Cassandra\BatchStatement`](http://datastax.github.io/php-driver/api/class/Cassandra/BatchStatement/)

Cassandra 1.2 also supported batching, but only as a CQL feature, you had to build the batch as a string, and it didn't really play well with prepared statements.

### Result paging

If you're using Cassandra 2.0 or later you can page your query results by adding the `page_size` option to [`Cassandra\ExecutionOptions`](http://datastax.github.io/php-driver/api/class/Cassandra/ExecutionOptions/):

```php
<?php

$statement = new Cassandra\SimpleStatement("SELECT * FROM large_table WHERE id = 'partition_with_lots_of_data'");
$result    = $session->execute($statement, new Cassandra\ExecutionOptions(array('page_size' => 100)));

while ($result) {
  foreach ($result as $row) {
    var_dump($row);
  }
  $result = $result->nextPage();
}
```

[Read more about `Cassandra\Rows::nextPage()`](http://datastax.github.io/php-driver/api/class/Cassandra/Rows/#next-page)

### Consistency

You can specify the default consistency to use when you create a new `Cassandra\Cluster`:

```php
<?php

$cluster = Cassandra::cluster()
               ->withDefaultConsistency(Cassandra::CONSISTENCY_LOCAL_QUORUM)
               ->build();
$session = $cluster->connect();
```

[Read more `Cassandra\Cluster\Builder::withDefaultConsistency()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-default-consistency)

Consistency can also be passed via `Cassandra\ExecutionOptions`.

```php
<?php

$session->execute(
  new Cassandra\SimpleStatement('SELECT * FROM users'),
  new Cassandra\ExecutionOptions(array('consistency' => Cassandra::CONSISTENCY_LOCAL_QUORUM))
);

$statement = $session->prepare('SELECT * FROM users');
$session->execute($statement, new Cassandra\ExecutionOptions(array(
  'consistency' => Cassandra::CONSISTENCY_LOCAL_QUORUM
)));

$batch = new Cassandra\BatchStatement();
$batch->add(new Cassandra\SimpleStatement("UPDATE users SET email = 'sue@foobar.com' WHERE id = 'sue'"));
$batch->add(new Cassandra\SimpleStatement("UPDATE users SET email = 'tom@foobar.com' WHERE id = 'tom'"));
$session->execute($batch, new Cassandra\ExecutionOptions(array(
  'consistency' => Cassandra::CONSISTENCY_LOCAL_QUORUM
)));
```

[Read more about `Cassandra\ExecutionOptions`](http://datastax.github.io/php-driver/api/class/Cassandra/ExecutionOptions/)
[Read more about `Cassandra\Session::execute()`](http://datastax.github.io/php-driver/api/interface/Cassandra/Session/#execute)

The default consistency level unless you've set it yourself is `Cassandra::CONSISTENCY_ONE`.

Consistency is ignored for `USE`, `TRUNCATE`, `CREATE` and `ALTER` statements, and some (like `Cassandra::CONSISTENCY_ANY`) aren't allowed in all situations.

### Logging

You can configure the location of the log file for the driver as well as the log level using the following `php.ini` settings:

```ini
[cassandra]
cassandra.log=syslog
cassandra.log_level=INFO
```

You can specify any file path as `cassandra.log`. The special value `syslog` can be used to for the driver to use syslog for logging.

The possible log levels are:

* CRITICAL
* ERROR
* WARN
* INFO
* DEBUG
* TRACE

Most of the logging will be when the driver connects and discovers new nodes, when connections fail and so on. The logging is designed to not cause much overhead and only relatively rare events are logged (e.g. normal requests are not logged).

## Architecture

The PHP Driver follows the architecture or [the C/C++ Driver](http://datastax.github.io/cpp-driver/topics/#architecture) that it wraps.

### Persistent Sessions

By default, the driver uses persistent sessions to prevent each request from creating completely new TCP connections to a Cassandra cluster. You can toggle this functionality using [`Cassandra\Cluster\Builder::withPersistentSessions`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-persistent-sessions)
