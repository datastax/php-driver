@cassandra-version-3.10
Feature: Duration

  PHP Driver supports the `Duration` datatype in collections

  Background:
    Given a running Cassandra cluster

  Scenario: Use the duration type in collections
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TYPE duration_user_type (duration1 duration, duration2 duration);
      CREATE TABLE durations (k text PRIMARY KEY,
                              duration_list list<duration>,
                              duration_tuple tuple<duration, duration>,
                              duration_udt duration_user_type);
      """
    And the following example:
      """
      <?php
      $cluster = Cassandra::cluster()->build();
      $session = $cluster->connect("simplex");

      # Construct collection arguments

      $list = Cassandra\Type::collection(Cassandra\Type::duration())->create(
          new Cassandra\Duration(1, 2, 3)
      );
      $list->add(new Cassandra\Duration(4, 5, 6));

      $tuple = Cassandra\Type::tuple(Cassandra\Type::duration(), Cassandra\Type::duration())->create(
          new Cassandra\Duration(1, 2, 3), new Cassandra\Duration(4, 5, 6)
      );

      $udt = Cassandra\Type::userType('duration1', Cassandra\Type::duration(), 'duration2', Cassandra\Type::duration())->create(
          'duration1', new Cassandra\Duration(1, 2, 3)
      );
      $udt->set('duration2', new Cassandra\Duration(4, 5, 6));


      # Insert collections containing durations into table
      $options = array('arguments' =>
          array('key1', $list, $tuple, $udt)
      );
      $session->execute("INSERT INTO durations
          (k, duration_list, duration_tuple, duration_udt) VALUES
          (?, ?, ?, ?)", $options);
      # Select collections from table and print as WKT
      $rows = $session->execute("SELECT * FROM durations WHERE k = 'key1'");
      $row = $rows->first();


      $rows = $session->execute("SELECT * FROM durations WHERE k = 'key1'");
      $row = $rows->first();

      echo 'The list contains durations: [' . implode(', ', $row['duration_list']->values()) . ']' . PHP_EOL;
      echo 'The tuple contains durations: (' . implode(', ', $row['duration_tuple']->values()) . ')' . PHP_EOL;
      $asWkt = array_map(function($k, $v) { return $k . ': ' . $v; },
          array_keys($row["duration_udt"]->values()), $row["duration_udt"]->values());
      echo 'The udt contains durations: {' . implode(', ', $asWkt) . '}' . PHP_EOL;
      """
    When it is executed
    Then its output should contain these lines in any order:
      """
      The list contains durations: [1mo2d3ns, 4mo5d6ns]
      The tuple contains durations: (1mo2d3ns, 4mo5d6ns)
      The udt contains durations: {duration1: 1mo2d3ns, duration2: 4mo5d6ns}
      """

