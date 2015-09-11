Feature: Schema Metadata

  PHP Driver exposes the Cassandra Schema Metadata for keyspaces, tables, and
  columns.

  Background:
    Given a running Cassandra cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      } AND DURABLE_WRITES = false;
      USE simplex;
      CREATE TABLE values (
        id int PRIMARY KEY,
        bigint_value bigint,
        decimal_value decimal,
        double_value double,
        float_value float,
        int_value int,
        varint_value varint,
        ascii_value ascii,
        text_value text,
        varchar_value varchar,
        timestamp_value timestamp,
        blob_value blob,
        uuid_value uuid,
        timeuuid_value timeuuid,
        inet_value inet,
        list_value List<text>,
        map_value Map<timestamp, double>,
        set_value Set<float>
      ) WITH
          bloom_filter_fp_chance=0.5 AND
          caching='ALL' AND
          comment='Schema Metadata Feature' AND
          compaction={'class': 'LeveledCompactionStrategy', 'sstable_size_in_mb' : 37} AND
          compression={'sstable_compression': 'DeflateCompressor'} AND
          dclocal_read_repair_chance=0.25 AND
          gc_grace_seconds=3600 AND
          populate_io_cache_on_flush='true' AND
          read_repair_chance=0.75 AND
          replicate_on_write='false';
      """

  Scenario: Keyspace metadata will exist for all major Cassandra versions
    Given the following example:
      """php
      <?php
      $cluster  = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session  = $cluster->connect("simplex");
      
      $schema   = $session->schema();
      $keyspace = $schema->keyspace("simplex");

      echo "Name: " . $keyspace->name() . "\n";
      echo "Replication Class: " . $keyspace->replicationClassName() . "\n";
      foreach ($keyspace->replicationOptions() as $key => $value) {
        echo "  " . $key . ":" . $value . "\n";
      }
      echo "Has Durable Writes: " . ($keyspace->hasDurableWrites() ? "True" : "False" ). "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Name: simplex
      Replication Class: org.apache.cassandra.locator.SimpleStrategy
        replication_factor:1
      Has Durable Writes: False
      """

    Scenario: Table metadata will exist for all major Cassandra versions
    Given the following example:
      """php
      <?php
      $cluster = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Name: " . $table->name() . "\n";
      echo "Bloom Filter: " . $table->bloomFilterFPChance() . "\n";
      $patterns = array();
      $patterns[0] = '/{/';
      $patterns[1] = '/"/';
      $patterns[2] = '/:/';
      $patterns[3] = '/keys/';
      $table_caching = explode(",", $table->caching());
      echo "Caching: " . preg_replace($patterns, "", $table_caching[0]) . "\n";
      echo "Comment: " . $table->comment() . "\n";
      echo "Compaction Class: " . $table->compactionStrategyClassName() . "\n";
      foreach ($table->compactionStrategyOptions() as $key => $value) {
        echo "  " . $key . ":" . $value . "\n";
      }
      foreach ($table->compressionParameters() as $key => $value) {
        echo "  " . $key . ":" . $value . "\n";
      }
      echo "DC/Local Read Repair Chance: " . $table->localReadRepairChance() . "\n";
      echo "Garbage Collection Grace Seconds: " . $table->gcGraceSeconds() . "\n";
      echo "Read Repair Chance: " . $table->readRepairChance() . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Name: values
      Bloom Filter: 0.5
      Caching: ALL
      Comment: Schema Metadata Feature
      Compaction Class: org.apache.cassandra.db.compaction.LeveledCompactionStrategy
        sstable_size_in_mb:37
        sstable_compression:org.apache.cassandra.io.compress.DeflateCompressor
      DC/Local Read Repair Chance: 0.25
      Garbage Collection Grace Seconds: 3600
      Read Repair Chance: 0.75
      """

    @cassandra-version-less-2.1
    Scenario: Additional table metadata will only exist for Cassandra versions
              1.2 and v2.0
    Given the following example:
      """php
      <?php
      $cluster = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Populate I/O Cache on Flush: " . ($table->populateIOCacheOnFlush() ? "True" : "False") . "\n";
      echo "Replicate on Write: " . ($table->replicateOnWrite() ? "True" : "False") . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Populate I/O Cache on Flush: True
      Replicate on Write: False
      """

    @cassandra-version-only-2.0
    Scenario: Additional table metadata will only exist for Cassandra versions
              2.0
    Given the following example:
      """php
      <?php
      $cluster = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      $cql = "ALTER TABLE values WITH index_interval = '512'";
      $session->execute(new Cassandra\SimpleStatement($cql));
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Index Interval: " . $table->indexInterval() . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Index Interval: 512
      """

    @cassandra-version-2.0
    Scenario: Additional table metadata will only exist for Cassandra versions
              2.0+
    Given the following example:
      """php
      <?php
      $cluster = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      $cql = "ALTER TABLE values WITH default_time_to_live = '10000' AND "
             . "memtable_flush_period_in_ms = '100' AND "
             . "speculative_retry = '10ms'";
      $session->execute(new Cassandra\SimpleStatement($cql));
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Default TTL: " . $table->defaultTTL() . "\n";
      echo "Memtable Flush Period: " . $table->memtableFlushPeriodMs() . "\n";
      echo "Speculative Retry: " . intval($table->speculativeRetry()) . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Default TTL: 10000
      Memtable Flush Period: 100
      Speculative Retry: 10
      """

    @cassandra-version-2.1
    Scenario: Additional table metadata will only exist for Cassandra versions
              2.1+
    Given the following example:
      """php
      <?php
      $cluster = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      $cql = "ALTER TABLE values WITH max_index_interval = '16' AND "
             . "min_index_interval = '4'";
      $session->execute(new Cassandra\SimpleStatement($cql));
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Maximum Index Interval: " . $table->maxIndexInterval() . "\n";
      echo "Minimum Index Interval: " . $table->minIndexInterval() . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      Maximum Index Interval: 16
      Minimum Index Interval: 4
      """

    @cassandra-version-2.0
    Scenario: Data types can be determined by the column metadata for Cassandra
              versions 2.0+
    Given the following example:
      """php
      <?php
      $cluster   = Cassandra::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session   = $cluster->connect("simplex");
      $schema    = $session->schema();
      $table     = $schema->keyspace("simplex")->table("values");
      $id        = $table->column("id")->type();
      $bigint    = $table->column("bigint_value")->type();
      $decimal   = $table->column("decimal_value")->type();
      $double    = $table->column("double_value")->type();
      $float     = $table->column("float_value")->type();
      $int       = $table->column("int_value")->type();
      $varint    = $table->column("varint_value")->type();
      $ascii     = $table->column("ascii_value")->type();
      $text      = $table->column("text_value")->type();
      $varchar   = $table->column("varchar_value")->type();
      $timestamp = $table->column("timestamp_value")->type();
      $blob      = $table->column("blob_value")->type();
      $uuid      = $table->column("uuid_value")->type();
      $timeuuid  = $table->column("timeuuid_value")->type();
      $inet      = $table->column("inet_value")->type();
      $list      = $table->column("list_value")->type();
      $map       = $table->column("map_value")->type();
      $set       = $table->column("set_value")->type();

      echo $id . "\n";
      echo $bigint . "\n";
      echo $decimal . "\n";
      echo $double . "\n";
      echo $float . "\n";
      echo $int . "\n";
      echo $varint . "\n";
      echo $ascii . "\n";
      echo $text . "\n";
      echo $varchar . "\n";
      echo $timestamp . "\n";
      echo $blob . "\n";
      echo $uuid . "\n";
      echo $timeuuid . "\n";
      echo $inet . "\n";
      echo $list . "\n";
      echo $map . "\n";
      echo $set . "\n";
      """
    When it is executed
    Then its output should contain:
      """
      int
      bigint
      decimal
      double
      float
      int
      varint
      ascii
      varchar
      varchar
      timestamp
      blob
      uuid
      timeuuid
      inet
      list<varchar>
      map<timestamp, double>
      set<float>
      """
