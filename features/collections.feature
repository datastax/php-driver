Feature: Collections

  PHP Driver supports all Cassandra collections

  Background:
    Given a running Cassandra cluster

  Scenario: Using Cassandra collections
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE user (
        id int PRIMARY KEY,
        logins list<timestamp>,
        locations map<timestamp, double>,
        ip_addresses set<inet>
      );
      INSERT INTO user (id, logins, locations, ip_addresses)
      VALUES (
        0,
        ['2014-09-11 10:09:08+0000', '2014-09-12 10:09:00+0000'],
        {'2014-09-11 10:09:08+0000': 37.397357},
        {'200.199.198.197', '192.168.1.15'}
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");
      $result    = $session->execute("SELECT * FROM user");
      $row       = $result->first();

      echo "Logins:" . PHP_EOL;
      foreach ($row['logins'] as $login) {
          echo "  {$login}" . PHP_EOL;
      }
      echo "Locations:" . PHP_EOL;
      foreach ($row['locations'] as $key => $location) {
          echo "  {$key} => {$location}" . PHP_EOL;
      }
      echo "Ip Addresses:" . PHP_EOL;
      foreach ($row['ip_addresses'] as $ip_address) {
          echo "  {$ip_address}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Logins:
        1410430148000
        1410516540000
      Locations:
        1410430148000 => 37.397357
      Ip Addresses:
        192.168.1.15
        200.199.198.197
      """

  @cassandra-version-2.1
  Scenario: Using Cassandra nested collections
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
        addresses map<text, frozen<map<text, text>>>
      );
      """
    And the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()->build();
      $session   = $cluster->connect("simplex");

      $addressType = Cassandra\Type::map(Cassandra\Type::text(), Cassandra\Type::text());
      $addressesType = Cassandra\Type::map(Cassandra\Type::text(), $addressType);

      $users = array(
          array(
              new Cassandra\Uuid('56357d2b-4586-433c-ad24-afa9918bc415'),
              'Charles Wallace',
              $addressesType->create(
                  'home', $addressType->create(
                      'city', 'Phoenix',
                      'street', '9042 Cassandra Lane',
                      'zip', '85023'))
          ),
          array(
              new Cassandra\Uuid('ce359590-8528-4682-a9f3-add53fc9aa09'),
              'Kevin Malone',
              $addressesType->create(
                  'home', $addressType->create(
                      'city', 'New York',
                      'street', '1000 Database Road',
                      'zip', '10025')
              )
          ),
          array(
              new Cassandra\Uuid('7d64dca1-dd4d-4f3c-bec4-6a88fa082a13'),
              'Michael Scott',
              $addressesType->create(
                  'work', $addressType->create(
                      'city', 'Santa Clara',
                      'street', '20000 Log Ave',
                      'zip', '95054'))
          )
      );

      foreach ($users as $user) {
          $options = array('arguments' => $user);
          $session->execute("INSERT INTO users (id, name, addresses) VALUES (?, ?, ?)", $options);
      }

      $result = $session->execute("SELECT * FROM users");

      foreach ($result as $row) {
          echo "ID: {$row['id']}" . PHP_EOL;
          echo "Name: {$row['name']}" . PHP_EOL;
          echo "Addresses" . PHP_EOL;
          $addresses = $row['addresses'];
          foreach ($addresses->keys() as $type) {
              echo "  {$type}:" . PHP_EOL;
              foreach ($addresses->get($type) as $name => $value) {
                  echo "    {$name} => {$value}" . PHP_EOL;
              }
          }
      }
      """
    When it is executed
    Then its output should contain:
      """
      ID: 56357d2b-4586-433c-ad24-afa9918bc415
      Name: Charles Wallace
      Addresses
        home:
          city => Phoenix
          street => 9042 Cassandra Lane
          zip => 85023
      ID: ce359590-8528-4682-a9f3-add53fc9aa09
      Name: Kevin Malone
      Addresses
        home:
          city => New York
          street => 1000 Database Road
          zip => 10025
      ID: 7d64dca1-dd4d-4f3c-bec4-6a88fa082a13
      Name: Michael Scott
      Addresses
        work:
          city => Santa Clara
          street => 20000 Log Ave
          zip => 95054
      """
