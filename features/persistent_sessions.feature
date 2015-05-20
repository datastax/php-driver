Feature: Persistent Sessions

  PHP Driver sessions can persist

  Background:
    Given a running Cassandra cluster
    And a web server on "127.0.0.1:4321"

  Scenario: A persistent session can be created
    Given a web server request on "/create":
      """
      <?php
      $cluster = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->withPersistentSessions(true)
                         ->build();
      $session = $cluster->connect();
      """
    And a web server request on "/info":
      """
      <?php
      phpinfo();
      """
    Then the request output should contain:
      """
      Persistent Sessions 1
      """
