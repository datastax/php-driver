@cassandra-version-2.1
Feature: Client-side timestamps

  Cassandra uses timestamps to serialize write operations. That is, values with
  more current timestamps are considered to be the most up-to-date version of
  that information. By default, timestamps are assigned by Cassandra on the
  server-side (`Cassandra\TimestampGenerator\ServerSide()`). The PHP Driver also
  allows for timestamps to be assigned client-side in two ways:

  * By providing a client-side timestamp generator such as
    `Cassandra\TimestampGenerator\Monotonic()`.
  * Explicitly assigning a 'timestamp' when executing a statement or batch
    using execution options.

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
      """

  Scenario: Create a simple statement with a client-side timestamp
    Given the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->build();
      $session   = $cluster->connect("simplex");

      $arguments = array(
          'song_id' => new Cassandra\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
          'title'   => 'La Petite Tonkinoise',
          'album'   => 'Bye Bye Blackbird',
          'artist'  => 'Joséphine Baker'
      );
      $options = array(
          'arguments' => $arguments,
          'timestamp' => 1234
      );
      $session->execute(
          "INSERT INTO playlists (id, song_id, artist, title, album) " .
          "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)",
          $options
      );

      $result = $session->execute("SELECT artist, title, album, WRITETIME(song_id) FROM simplex.playlists");

      foreach ($result as $row) {
        echo $row['artist'] . ": " . $row['title'] . " / " . $row['album'] . " (". $row['writetime(song_id)'] . ")\n";
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird (1234)
      """

  Scenario: Create a batch with a client-side timestamp
    Given the following example:
      """php
      <?php
      $cluster     = Cassandra::cluster()->build();
      $session     = $cluster->connect("simplex");
      $insertQuery = "INSERT INTO playlists (id, song_id, artist, title, album) " .
                     "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)";
      $prepared    = $session->prepare($insertQuery);
      $batch       = new Cassandra\BatchStatement(Cassandra::BATCH_LOGGED);

      $batch->add($prepared, array(
          'song_id' => new Cassandra\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
          'title'   => 'La Petite Tonkinoise',
          'album'   => 'Bye Bye Blackbird',
          'artist'  => 'Joséphine Baker'
      ));

      $batch->add($insertQuery, array(
          new Cassandra\Uuid('f6071e72-48ec-4fcb-bf3e-379c8a696488'),
          'Willi Ostermann', 'Die Mösch', 'In Gold',
      ));

      $batch->add($prepared, array(
          new Cassandra\Uuid('fbdf82ed-0063-4796-9c7c-a3d4f47b4b25'),
          'Mick Jager', 'Memo From Turner', 'Performance'
      ));

      $options = array("timestamp" => 5678);

      $session->execute($batch, $options);

      $result = $session->execute("SELECT artist, title, album, WRITETIME(song_id) FROM simplex.playlists");

      foreach ($result as $row) {
        echo $row['artist'] . ": " . $row['title'] . " / " . $row['album'] . " (". $row['writetime(song_id)'] . ")\n";
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird (5678)
      """
    And its output should contain:
      """
      Willi Ostermann: Die Mösch / In Gold (5678)
      """
    And its output should contain:
      """
      Mick Jager: Memo From Turner / Performance (5678)
      """

  Scenario: Create a new session using a timestamp generator
    Given the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->withTimestampGenerator(new Cassandra\TimestampGenerator\Monotonic())
                     ->build();
      $session   = $cluster->connect("simplex");

      for ($i = 0; $i < 10; $i++) {
          $arguments = array(
              'song_id' => new Cassandra\Uuid("756716f7-2e54-4715-9f00-91dcbea6cf5$i"),
              'title'   => 'La Petite Tonkinoise',
              'album'   => 'Bye Bye Blackbird',
              'artist'  => 'Joséphine Baker'
          );
          $options = array(
              'arguments' => $arguments,
              'timestamp' => 1234
          );
          $session->execute(
              "INSERT INTO playlists (id, song_id, artist, title, album) " .
              "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)",
              $options
          );
      }

      $result = $session->execute("SELECT artist, title, album, song_id FROM simplex.playlists");

      foreach ($result as $row) {
        echo $row['artist'] . ": " . $row['title'] . " / " . $row['album'] . " (". $row['song_id'] . ")\n";
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird (756716f7-2e54-4715-9f00-91dcbea6cf59)
      """
