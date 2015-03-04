--TEST--
--SKIPIF--
<?php if (!extension_loaded("cassandra")) die("Skipped: cassandra extension required."); ?>
--FILE--
<?php
$cluster = cassandra_cluster_new();
$session = cassandra_session_new();
$f = $future  = cassandra_session_connect($session, $cluster);

echo $future;
echo "\n";
unset($future);
unset($session);
unset($cluster);
cassandra_future_free($f);
echo "success";
?>
--EXPECT--
Resource id #6
success
