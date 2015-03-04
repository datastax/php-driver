--TEST--
--SKIPIF--
<?php ?>
--FILE--
<?php
$cluster = cassandra_cluster_new();
echo $cluster;
echo "\n";
cassandra_cluster_free($cluster);
echo "success";
?>
--EXPECT--
Resource id #4
success
