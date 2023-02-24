@cassandra-version-2.1
Feature: Tuples

  PHP Driver supports Cassandra tuples

  Background:
    Given a running Cassandra cluster

  Scenario: Using Cassandra tuples
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE users (
        id uuid PRIMARY KEY,
        name text,
        address tuple<text, text, int>
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");

      $tupleType = Cassandra\Type::tuple(Cassandra\Type::text(), Cassandra\Type::text(), Cassandra\Type::int());

      $users = array(
          array(
              new Cassandra\Uuid('56357d2b-4586-433c-ad24-afa9918bc415'),
              'Charles Wallace',
              $tupleType->create('Phoenix', '9042 Cassandra Lane', 85023)
          ),
          array(
              new Cassandra\Uuid('ce359590-8528-4682-a9f3-add53fc9aa09'),
              'Kevin Malone',
              $tupleType->create('New York', '1000 Database Road', 10025)
          ),
          array(
              new Cassandra\Uuid('7d64dca1-dd4d-4f3c-bec4-6a88fa082a13'),
              'Michael Scott',
              $tupleType->create('Santa Clara', '20000 Log Ave', 95054)
          )
      );

      foreach ($users as $user) {
          $options = array('arguments' => $user);
          $session->execute("INSERT INTO users (id, name, address) VALUES (?, ?, ?)", $options);
      }

      $result    = $session->execute("SELECT * FROM users");

      foreach ($result as $row) {
          echo "ID: {$row['id']}" . PHP_EOL;
          echo "Name: {$row['name']}" . PHP_EOL;
          echo "Address:" . PHP_EOL;
          foreach ($row['address'] as $format) {
              echo "  {$format}" . PHP_EOL;
          }
      }
      """
    When it is executed
    Then its output should contain:
      """
      ID: 56357d2b-4586-433c-ad24-afa9918bc415
      Name: Charles Wallace
      Address:
        Phoenix
        9042 Cassandra Lane
        85023
      ID: ce359590-8528-4682-a9f3-add53fc9aa09
      Name: Kevin Malone
      Address:
        New York
        1000 Database Road
        10025
      ID: 7d64dca1-dd4d-4f3c-bec4-6a88fa082a13
      Name: Michael Scott
      Address:
        Santa Clara
        20000 Log Ave
        95054
      """
