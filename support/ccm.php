<?php

use Symfony\Component\Process\Process;
use Cassandra\SimpleStatement;

class CCM
{
    private $name;
    private $version;
    private $process;
    private $cluster;
    private $session;

    public function __construct($name, $version)
    {
        $this->name    = $name;
        $this->version = $version;
        $this->process = new Process(null);
        $this->cluster = null;
        $this->session = null;
    }

    public function setupSchema($schema)
    {
        $keyspaces = $this->session->execute(new SimpleStatement("SELECT keyspace_name FROM system.schema_keyspaces"));

        foreach ($keyspaces as $row) {
            $keyspace = $row['keyspace_name'];

            if ($this->startsWith("system", $keyspace)) {
                continue;
            }

            echo "DROP KEYSPACE " . $keyspace . "\n";
            $this->session->execute(new SimpleStatement("DROP KEYSPACE $keyspace"));
        }

        foreach (explode(";", $schema) as $cql) {
            $cql = trim($cql);

            if (empty($cql)) {
                continue;
            }

            echo $cql . "\n";
            $this->session->execute(new SimpleStatement($cql));
        }
    }

    public function start()
    {
        $this->setup();
        $this->run('start');
        $this->cluster = Cassandra::cluster()->build();
        $this->session = $this->cluster->connect();
    }

    private function setup()
    {
        $clusters = array();
        foreach (explode("\n", $this->run('list')) as $cluster) {
            if ($this->isActive($cluster)) {
                $active = count($clusters);
            }
            $clusters[] = substr($cluster, 2, strlen($cluster) - 2);
        }

        if (isset($active) && $clusters[$active] == $this->name) {
            return;
        }

        if (in_array($this->name, $clusters)) {
            $this->run('switch', $this->name);
            return;
        }

        $this->run('create', '-v', 'binary:' . $this->version, '-b', $this->name);

        $params = array(
          'updateconf', '--rt', '1000', 'read_request_timeout_in_ms: 1000',
          'write_request_timeout_in_ms: 1000', 'request_timeout_in_ms: 1000',
          'phi_convict_threshold: 16', 'hinted_handoff_enabled: false',
          'dynamic_snitch_update_interval_in_ms: 1000'
        );

        if (substr($this->version, 0, 4) == '1.2.') {
            $params[] = 'reduce_cache_sizes_at: 0';
            $params[] = 'reduce_cache_capacity_to: 0';
            $params[] = 'flush_largest_memtables_at: 0';
            $params[] = 'index_interval: 512';
        } else {
            $params[] = 'cas_contention_timeout_in_ms: 10000';
            $params[] = 'file_cache_size_in_mb: 0';
        }

        $params[] = 'native_transport_max_threads: 1';
        $params[] = 'rpc_min_threads: 1';
        $params[] = 'rpc_max_threads: 1';
        $params[] = 'concurrent_reads: 2';
        $params[] = 'concurrent_writes: 2';
        $params[] = 'concurrent_compactors: 1';
        $params[] = 'compaction_throughput_mb_per_sec: 0';

        if (strcmp($this->version, '2.1') < 0) {
            $params[] = 'in_memory_compaction_limit_in_mb: 1';
        }

        $params[] = 'key_cache_size_in_mb: 0';
        $params[] = 'key_cache_save_period: 0';
        $params[] = 'memtable_flush_writers: 1';
        $params[] = 'max_hints_delivery_threads: 1';

        call_user_func_array(array($this, 'run'), $params);
        $this->run('populate', '-n', '1', '-i', '127.0.0.');
    }

    private function isActive($clusterName)
    {
        return $this->startsWith(' *', $clusterName);
    }

    private function startsWith($prefix, $string)
    {
        return substr($string, 0, strlen($prefix)) === $prefix;
    }

    private function run()
    {
        $args = func_get_args();
        foreach ($args as $i => $arg) {
          $args[$i] = escapeshellarg($arg);
        }

        $command = sprintf('ccm %s', implode(' ', $args));
        $this->process->setCommandLine($command);

        echo 'ccm > ' . $command . "\n";
        $this->process->mustRun(function ($type, $buffer) {
            echo 'ccm > ' . $buffer;
        });

        return $this->process->getOutput();
    }
}
