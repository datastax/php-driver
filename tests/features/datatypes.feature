Feature: Datatypes

  PHP Driver supports all Cassandra datatypes

  Background:
    Given a running Cassandra cluster

  Scenario: Using Cassandra value types
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE values (
        id int PRIMARY KEY,
        bigint_value bigint,
        decimal_value decimal,
        double_value double,
        float_value float,
        int_value int,
        varint_value varint,
        timestamp_value timestamp,
        blob_value blob,
        uuid_value uuid,
        timeuuid_value timeuuid,
        inet_value inet
      );
      INSERT INTO values (
        id,
        bigint_value,
        decimal_value,
        double_value,
        float_value,
        int_value,
        varint_value,
        timestamp_value,
        blob_value,
        uuid_value,
        timeuuid_value,
        inet_value
      )
      VALUES (
        0,
        -765438000,
        1313123123.234234234234234234123,
        3.141592653589793,
        3.14,
        4,
        67890656781923123918798273492834712837198237,
        1425691864001,
        varcharAsBlob('0x000000'),
        ab3352d9-4f7f-4007-a35a-e62aa7ab0b19,
        maxTimeuuid('2015-03-11 14:47:10+0000'),
        '200.199.198.197'
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");
      $result    = $session->execute("SELECT * FROM values");
      $row       = $result->first();

      foreach ($row as $name => $value) {
          if ($name !== "id") {
              echo "{$name} => {$value}" .PHP_EOL;
          }
      }
      """
    When it is executed
    Then its output should contain these lines in any order:
      """
      bigint_value => -765438000
      blob_value => 0x3078303030303030
      decimal_value => 1313123123.234234234234234234123
      double_value => 3.1415926535898
      float_value => 3.14000010490417
      inet_value => 200.199.198.197
      int_value => 4
      timestamp_value => 1425691864001
      timeuuid_value => 7f0a920f-c7fd-11e4-7f7f-7f7f7f7f7f7f
      uuid_value => ab3352d9-4f7f-4007-a35a-e62aa7ab0b19
      varint_value => 67890656781923123918798273492834712837198237
      """

  @cassandra-version-2.2
  @tinyint
  @smallint
  Scenario: Using Cassandra tinyint and smallint types
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE values (
        id int PRIMARY KEY,
        tinyint_value tinyint,
        smallint_value smallint,
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");

      $options   = array('arguments' =>
        array(1, new Cassandra\Tinyint(127), new Cassandra\Smallint(32767))
      );
      $session->execute("INSERT INTO values (id, tinyint_value, smallint_value) VALUES (?, ?, ?)", $options);

      $result    = $session->execute("SELECT * FROM values");
      $row       = $result->first();

      foreach ($row as $name => $value) {
          if ($name != "id") {
              echo "{$name} => {$value}" .PHP_EOL;
          }
      }
      """
    When it is executed
    Then its output should contain:
      """
      smallint_value => 32767
      tinyint_value => 127
      """

  @cassandra-version-2.2
  @date
  Scenario: Using Cassandra date type
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE date_values (
        id int PRIMARY KEY,
        date_value date
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");

      $options   = array('arguments' => array(1, new Cassandra\Date(0)));
      $session->execute("INSERT INTO date_values (id, date_value) VALUES (?, ?)", $options);

      $result    = $session->execute("SELECT * FROM date_values");
      $row       = $result->first();

      echo "date_value => {$row['date_value']->toDateTime()->format('Y-m-d H:i:s')}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      date_value => 1970-01-01 00:00:00
      """

  @cassandra-version-2.2
  @time
  Scenario: Using Cassandra time type
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE time_values (
        id int PRIMARY KEY,
        time_value time
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");

      $datetime = new \DateTime("1970-01-01T00:00:01+0000");
      $options   = array('arguments' => array(1, Cassandra\Time::fromDateTime($datetime)));
      $session->execute("INSERT INTO time_values (id, time_value) VALUES (?, ?)", $options);

      $result    = $session->execute("SELECT * FROM time_values");
      $row       = $result->first();

      echo "time_value => {$row['time_value']}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      time_value => 1000000000
      """
