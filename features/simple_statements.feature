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
      use Cassandra

      $cluster = Cassandra::cluster()
                   ->build();
      $session = $cluster->connect("simplex");
      $result  = $session.execute("SELECT * FROM simplex.playlists");
      echo "Result contains " . count($result) . " rows";
      """
    When it is executed
    Then its output should contain:
      """
      Result contains 0 rows
      """
