Feature: Simple string queries

  PHP Driver supports running string queries.

  Background:
    Given a running Cassandra cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
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

      INSERT INTO playlists
      (id, song_id, artist, title, album) VALUES
      (62c36092-82a1-3a00-93d1-46196ee77204, 756716f7-2e54-4715-9f00-91dcbea6cf50, 'Joséphine Baker', 'La Petite Tonkinoise', 'Bye Bye Blackbird');

      INSERT INTO playlists
      (id, song_id, artist, title, album) VALUES
      (62c36092-82a1-3a00-93d1-46196ee77204, f6071e72-48ec-4fcb-bf3e-379c8a696488, 'Willi Ostermann', 'Die Mösch', 'In Gold');

      INSERT INTO playlists
      (id, song_id, artist, title, album) VALUES
      (62c36092-82a1-3a00-93d1-46196ee77204, fbdf82ed-0063-4796-9c7c-a3d4f47b4b25, 'Mick Jager', 'Memo From Turner', 'Performance');
      """

  Scenario: A simple CQL string can be used to execute queries
    Given the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->build();
      $session   = $cluster->connect("simplex");
      $result    = $session->execute("SELECT * FROM playlists");

      foreach ($result as $row) {
        echo $row['artist'] . ": " . $row['title'] . " / " . $row['album'] . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird
      """
    And its output should contain:
      """
      Willi Ostermann: Die Mösch / In Gold
      """
    And its output should contain:
      """
      Mick Jager: Memo From Turner / Performance
      """

  Scenario: A simple CQL string can also be used to execute asynchronous queries
    Given the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->build();
      $session   = $cluster->connect("simplex");
      $future    = $session->executeAsync("SELECT * FROM playlists");

      echo "Doing something else..." . PHP_EOL;

      $result = $future->get();

      foreach ($result as $row) {
        echo $row['artist'] . ": " . $row['title'] . " / " . $row['album'] . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Doing something else...
      """
    And its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird
      """
    And its output should contain:
      """
      Willi Ostermann: Die Mösch / In Gold
      """
    And its output should contain:
      """
      Mick Jager: Memo From Turner / Performance
      """
