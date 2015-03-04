--TEST--
--SKIPIF--
<?php if (!extension_loaded("cassandra")) die("Skipped: cassandra extension required."); ?>
--FILE--
<?php
$s = $session = cassandra_session_new();
echo $session;
echo "\n";
unset($session);
cassandra_session_free($s);
echo "success";
?>
--EXPECT--
Resource id #4
success
