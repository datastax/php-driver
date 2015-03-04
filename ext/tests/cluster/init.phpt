--TEST--
--SKIPIF--
<?php if (!extension_loaded("cassandra")) die("Skipped: cassandra extension required."); ?>
--FILE--
<?php
$c = $cluster = cassandra_cluster_new();
echo $cluster;
echo "\n";
unset($cluster);
cassandra_cluster_free($c);
echo "success";
?>
--EXPECT--
Resource id #4
success
