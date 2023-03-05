@cassandra-version-3.10
Feature: Duration

  PHP Driver supports the `Duration` datatype

  Background:
    Given a running Cassandra cluster

  Scenario: Use the duration type
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE duration (k text PRIMARY KEY, d duration);
      """
    And the following example:
      """
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");

      $durations = array(
          array('two_days', new Cassandra\Duration(2, 0, 0)),
          array('twelve_hours', new Cassandra\Duration(0, 12, 0)),
          array('three_seconds', new Cassandra\Duration(0, 0, 3 * (1000 ** 3))),
          array('two_days_twelve_hours_and_three_seconds', new Cassandra\Duration(2, 12, 3 * (1000 ** 3)))
      );

      foreach ($durations as $duration) {
          $options = array('arguments' => $duration);
          $session->execute("INSERT INTO duration (k, d) VALUES (?, ?)", $options);
      }

      $rows = $session->execute("SELECT * FROM duration");

      foreach ($rows as $row) {
        echo "{$row['k']}: {$row['d']}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain these lines in any order:
      """
      twelve_hours: 0mo12d0ns
      three_seconds: 0mo0d3000000000ns
      two_days_twelve_hours_and_three_seconds: 2mo12d3000000000ns
      two_days: 2mo0d0ns
      """

