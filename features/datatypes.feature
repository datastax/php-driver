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
      )
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->build();
      $session   = $cluster->connect("simplex");
      $statement = new Cassandra\SimpleStatement("SELECT * FROM values");
      $result    = $session->execute($statement);
      $row       = $result->first();

      echo "Bigint: " . var_export($row['bigint_value'], true) . "\n";
      echo "Decimal: " . var_export($row['decimal_value'], true) . "\n";
      echo "Double: " . var_export($row['double_value'], true) . "\n";
      echo "Float: " . var_export($row['float_value'], true) . "\n";
      echo "Int: " . var_export($row['int_value'], true) . "\n";
      echo "Varint: " . var_export($row['varint_value'], true) . "\n";
      echo "Timestamp: " . var_export($row['timestamp_value'], true) . "\n";
      echo "Blob: " . var_export($row['blob_value'], true) . "\n";
      echo "Uuid: " . var_export($row['uuid_value'], true) . "\n";
      echo "Timeuuid: " . var_export($row['timeuuid_value'], true) . "\n";
      echo "Inet: " . var_export($row['inet_value'], true) . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Bigint: Cassandra\Bigint::__set_state(array(
         'value' => '-765438000',
      ))
      Decimal: Cassandra\Decimal::__set_state(array(
         'value' => '1313123123234234234234234234123',
         'scale' => 21,
      ))
      Double: 3.1415926535897931
      Float: Cassandra\Float::__set_state(array(
         'value' => '3.14000010490417',
      ))
      Int: 4
      Varint: Cassandra\Varint::__set_state(array(
         'value' => '67890656781923123918798273492834712837198237',
      ))
      Timestamp: Cassandra\Timestamp::__set_state(array(
         'seconds' => 1425691864,
         'microseconds' => 1000,
      ))
      Blob: Cassandra\Blob::__set_state(array(
         'bytes' => '0x3078303030303030',
      ))
      Uuid: Cassandra\Uuid::__set_state(array(
         'uuid' => 'ab3352d9-4f7f-4007-a35a-e62aa7ab0b19',
         'version' => 4,
      ))
      Timeuuid: Cassandra\Timeuuid::__set_state(array(
         'uuid' => '7f0a920f-c7fd-11e4-7f7f-7f7f7f7f7f7f',
         'version' => 1,
      ))
      Inet: Cassandra\Inet::__set_state(array(
         'address' => '200.199.198.197',
      ))
      """

  Scenario: Using Cassandra collection types
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE user (
        id int PRIMARY KEY,
        logins List<timestamp>,
        locations Map<timestamp, double>,
        ip_addresses Set<inet>
      );
      INSERT INTO user (id, logins, locations, ip_addresses)
      VALUES (
        0,
        ['2014-09-11 10:09:08+0000', '2014-09-12 10:09:00+0000'],
        {'2014-09-11 10:09:08+0000': 37.397357},
        {'200.199.198.197', '192.168.1.15'}
      )
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->build();
      $session   = $cluster->connect("simplex");
      $statement = new Cassandra\SimpleStatement("SELECT * FROM user");
      $result    = $session->execute($statement);
      $row       = $result->first();

      echo "Logins: " . var_export($row['logins'], true) . "\n";
      echo "Locations: " . var_export($row['locations'], true) . "\n";
      echo "Ip Addresses: " . var_export($row['ip_addresses'], true) . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Logins: Cassandra\Collection::__set_state(array(
         'values' =>
        array (
          0 =>
          Cassandra\Timestamp::__set_state(array(
             'seconds' => 1410430148,
             'microseconds' => 0,
          )),
          1 =>
          Cassandra\Timestamp::__set_state(array(
             'seconds' => 1410516540,
             'microseconds' => 0,
          )),
        ),
      ))
      Locations: Cassandra\Map::__set_state(array(
         'keys' =>
        array (
          0 =>
          Cassandra\Timestamp::__set_state(array(
             'seconds' => 1410430148,
             'microseconds' => 0,
          )),
        ),
         'values' =>
        array (
          0 => 37.397357,
        ),
      ))
      Ip Addresses: Cassandra\Set::__set_state(array(
         'values' =>
        array (
          0 =>
          Cassandra\Inet::__set_state(array(
             'address' => '192.168.1.15',
          )),
          1 =>
          Cassandra\Inet::__set_state(array(
             'address' => '200.199.198.197',
          )),
        ),
      ))
      """
