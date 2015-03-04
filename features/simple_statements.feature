Feature: Simple Statements

  PHP Driver supports simple statements

  Scenario: Usage example
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 3
      };
      USE simplex;
      CREATE TABLE playlists (
        id uuid,
        title text,
        album text,
        artist text,
        song_id uuid,
        PRIMARY KEY (id, title, album, artist)
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints(array('127.0.0.1'))
                     ->build();
      $session   = $cluster->connect("simplex");
      $statement = new Cassandra\SimpleStatement("SELECT * FROM simplex.playlists");
      $result    = $session->execute($statement);
      echo "Result contains " . count($result) . " rows";
      """
    When it is executed
    Then its output should contain:
      """
      Result contains 0 rows
      """
