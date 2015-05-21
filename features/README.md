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

After the initial connection to one of the hosts specified via `withContactPoints()` succeeded, the driver discovers the addresses and connects to all members of the cluster automatically. You can also see the nodes that the driver discovered by running `SELECT * FROM system.peers`.

### Persistent sessions

In order to limit the startup time and total number of connections to a Cassandra cluster, the PHP Driver enabled persistent sessions by default. All cluster and sessions using the same initial configuration will be shared across requests when persistent sessions are enabled. You can toggle this setting using [`Cassandra\Cluster\Builder::withPersistentSessions()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-persistent-sessions).

```php
<?php

$cluster = Cassandra::cluster()
               ->withPersistentSessions(false)
               ->build();
$session = $cluster->connect();
```

Note that disabling persistent sessions will cause significant slow down of cluster initialization as the connections will be forced to get re-established for every request.

Once persistent sessions are enabled, you can view how many of them are currently active. They will be exposed in the Cassandra extension section of `phpinfo()`.

### Configuring load balancing policy

PHP Driver comes with a variety of load balancing policies. By default it uses a combination of token aware data center round robin.

The token aware load balancing policy uses the same hashing algorithms as the Apache Cassandra to route prepared statements execution directly to replica nodes, avoiding an additional network hop to/from the coordinator. You can toggle its usage with [`Cassandra\Cluster\Builder::withTokenAwareRouting()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-token-aware-routing).

```php
<?php

$cluster = Cassandra::cluster()
               ->withTokenAwareRouting(false)
               ->build();
$session = $cluster->connect();
```

The default datacenter aware round robin load balancing policy is configured to keep all traffic in the same datacenter. Upon connecting to a host from the initial list of contact points, the driver will consider that host's datacenter to be local. Only hosts from the same datacenter will be connected to and used for executing statements. You can override the name of the local datacenter. The number of hosts from remote datacenters that the driver may use and whether it should execute statements with local consistencies on those hosts in case none of the local hosts are available. All of that is configurable via [`Cassandra\Cluster\Builder::withDatacenterAwareRoundRobinLoadBalancingPolicy()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-datacenter-aware-round-robin-load-balancing-policy).

```php
<?php

$cluster = Cassandra::cluster()
               ->withDatacenterAwareRoundRobinLoadBalancingPolicy("us-west", 2, true)
               ->build();
$session = $cluster->connect();
```

Finally, you may disable datacenter awareness by calling [`Cassandra\Cluster\Builder::withRoundRobinLoadBalancingPolicy()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-round-robin-load-balancing-policy).

```php
<?php

$cluster = Cassandra::cluster()
               ->withRoundRobinLoadBalancingPolicy()
               ->build();
$session = $cluster->connect();
```

### Authenticating via `PasswordAuthenticator`

The PHP Driver supports Apache Cassandra's built-in password authentication mechanism. To enable it, use [`Cassandra\Cluster\Builder::withCredentials()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-credentials).

```php
<?php

$cluster = Cassandra::cluster()
               ->withCredentials("username", "password")
               ->build();
$session = $cluster->connect();
```

### Enabling SSL encryption

The PHP Driver supports SSL encryption of network connections. You must configure [`Cassandra\SSLOptions`](http://datastax.github.io/php-driver/api/class/Cassandra/SSLOptions/) using the [`Cassandra\SSLOptions\Builder`](http://datastax.github.io/php-driver/api/class/Cassandra/SSLOptions/Builder/).

```php
<?php

$ssl     = Cassandra::ssl()
               ->withTrustedCerts('node1.pem', 'node2.pem')
               ->withVerifyFlags(Cassandra::VERIFY_PEER_CERT | Cassandra::VERIFY_PEER_IDENTITY)
               ->withClientCert('client.pem')
               ->withPrivateKey('id_rsa', 'passphrase')
               ->build()
$cluster = Cassandra::cluster()
               ->withSSL($ssl)
               ->build();
$session = $cluster->connect();
```

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

You can also `ALTER` keyspaces and tables, and you can read more about that in the [CQL3 syntax documentation](https://github.com/apache/cassandra/blob/cassandra-2.0/doc/cql3/CQL.textile).

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

Batches can have one of three different types: `logged`, `unlogged` or `counter`, where `logged` is the default. Their exact semantics are defined in the [Cassandra documentation](http://docs.datastax.com/en/cql/3.1/cql/cql_reference/batch_r.html), but this is how you specify which one you want:

```php
<?php

$batch = new Cassandra\BatchStatement(Cassandra::BATCH_LOGGED);
$batch = new Cassandra\BatchStatement(Cassandra::BATCH_UNLOGGED);
$batch = new Cassandra\BatchStatement(Cassandra::BATCH_COUNTER);
```

[Read more about `Cassandra\BatchStatement`](http://datastax.github.io/php-driver/api/class/Cassandra/BatchStatement/)

Cassandra 1.2 also supported batching, but only as a CQL feature, you had to build the batch as a string, and it didn't really play well with prepared statements.

### Result paging

**If you're using Cassandra 2.0** or later you can page your query results.

By default, a page size of 5000 will be used, you can override the default page size via [`Cassandra\Cluster\Builder::withDefaultPageSize()`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-default-page-size).

```php
<?php

$cluster = Cassandra::cluster()
               ->withDefaultPageSize(200)
               ->build();
$session = $cluster->connect();
```

You can also override the page size on a per-execute basis by adding the `page_size` option to [`Cassandra\ExecutionOptions`](http://datastax.github.io/php-driver/api/class/Cassandra/ExecutionOptions/):

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

The PHP Driver follows the architecture of [the C/C++ Driver](http://datastax.github.io/cpp-driver/topics/#architecture) that it wraps.

### Persistent Sessions

By default, the driver uses persistent sessions to prevent each request from creating completely new TCP connections to a Cassandra cluster. You can toggle this functionality using [`Cassandra\Cluster\Builder::withPersistentSessions`](http://datastax.github.io/php-driver/api/class/Cassandra/Cluster/Builder/#with-persistent-sessions)
