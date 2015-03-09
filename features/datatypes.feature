@wip
Feature: Datatypes

  PHP Driver supports all Cassandra datatypes

  Scenario: Cassandra bigint is represented as Cassandra\Bigint
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE numbers (
        id int PRIMARY KEY,
        bigint_value bigint,
        decimal_value decimal,
        double_value double,
        float_value float,
        int_value int,
        varint_value varint,
        timestamp_value timestamp,
        blob_value blob
      );
      INSERT INTO numbers (
        id,
        bigint_value,
        decimal_value,
        double_value,
        float_value,
        int_value,
        varint_value,
        timestamp_value,
        blob_value
      )
      VALUES (
        0,
        765438000,
        1313123123.234234234234234234123,
        3.141592653589793,
        3.14,
        4,
        67890656781923123918798273492834712837198237,
        1425691864001,
        varcharAsBlob('0x000000')
      )
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints(array('127.0.0.1'))
                     ->build();
      $session   = $cluster->connect("simplex");
      $statement = new Cassandra\SimpleStatement("SELECT * FROM simplex.numbers");
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
      """
    When it is executed
    Then its output should contain:
      """
      Bigint: Cassandra\Bigint::__set_state(array(
        'value' => '765438000',
      ))
      Decimal: Cassandra\Decimal::set_state(array(
        'value' => '1313123123.234234234234234234123',
      ))
      Double: 3.1415926535897931
      Float: 3.1400001049041748
      Int: 4
      Varint: Cassandra\Varint::__set_state(array(
        'value' => '67890656781923123918798273492834712837198237',
      ))
      Timestamp: Cassandra\Timestamp::__set_state(array(
        'seconds' => 1425691864,
        'microseconds' => 1000,
      ))
      Blob: Cassandra\Blob::__set_state(array(
        'bytes' => '0x000000',
      ))
      """
