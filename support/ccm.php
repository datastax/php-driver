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
    private $ssl;
    private $clientAuth;
    private $dataCenterOneNodes;
    private $dataCenterTwoNodes;

    public function __construct($name, $version)
    {
        $this->name               = $name;
        $this->version            = $version;
        $this->process            = new Process(null);
        $this->cluster            = null;
        $this->session            = null;
        $this->ssl                = false;
        $this->clientAuth         = false;
        $this->dataCenterOneNodes = 0;
        $this->dataCenterTwoNodes = 0;
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
        $this->run('start', '--wait-other-notice', '--wait-for-binary-proto');
        $builder = Cassandra::cluster()
                       ->withPersistentSessions(false)
                       ->withContactPoints('127.0.0.1');

        if ($this->ssl || $this->clientAuth) {
            $sslOptions = Cassandra::ssl()
                              ->withTrustedCerts(realpath(__DIR__ . '/ssl/cassandra.pem'))
                              ->withVerifyFlags(Cassandra::VERIFY_PEER_CERT)
                              ->withClientCert(realpath(__DIR__ . '/ssl/driver.pem'))
                              ->withPrivateKey(realpath(__DIR__ . '/ssl/driver.key'), 'php-driver')
                              ->build();
            $builder->withSSL($sslOptions);
        }

        for ($retries = 1; $retries <= 10; $retries++) {
            try {
                $this->cluster = $builder->build();
                $this->session = $this->cluster->connect();
            } catch (Cassandra\Exception\RuntimeException $e) {
                $this->cluster = null;
                $this->session = null;
                sleep($retries * 0.4);
            }
        }

        if (!isset($this->session)) {
            throw new RuntimeException("Unable to initialize a Session, check cassandra logs");
        }
    }

    public function stop()
    {
        if ($this->session) {
            $this->session->close();
            $this->session = null;
            $this->cluster = null;
        }
        $this->run('stop');
    }

    private function getClusters()
    {
        $active = '';
        $clusters = array();
        foreach (explode(PHP_EOL, $this->run('list')) as $cluster) {
            $clusterName = trim(substr($cluster, 2, strlen($cluster) - 2));

            // Determine if this cluster is the active cluster
            if ($this->isActive($cluster)) {
                $active = $clusterName;
            }

            // Add the cluster to the list
            if (!empty($clusterName)) {
                $clusters[] = $clusterName;
            }
        }

        return array('active' => $active, 'list' => $clusters);
    }

    public function setup($dataCenterOneNodes, $dataCenterTwoNodes)
    {
        $this->dataCenterOneNodes = $dataCenterOneNodes;
        $this->dataCenterTwoNodes = $dataCenterTwoNodes;

        $clusters = $this->getClusters();
        $clusterName = $this->name.'_'.$dataCenterOneNodes.'-'.$dataCenterTwoNodes;
        if ($clusters['active'] != $clusterName) {
            // Ensure any active cluster is stopped
            if (!empty($clusters['active'])) {
                $this->stop();
            }

            // Determine if a cluster should be created or re-used
            if (in_array($clusterName, $clusters['list'])) {
                $this->run('switch', $clusterName);
            } else {
                $this->run('create', '-v', 'binary:' . $this->version, '-b', $clusterName);

                $params = array(
                  'updateconf', '--rt', '1000', 'read_request_timeout_in_ms: 1000',
                  'write_request_timeout_in_ms: 1000', 'request_timeout_in_ms: 1000',
                  'phi_convict_threshold: 16', 'hinted_handoff_enabled: false',
                  'dynamic_snitch_update_interval_in_ms: 1000',
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
                $this->run('populate', '-n', $dataCenterOneNodes.':'.$dataCenterTwoNodes, '-i', '127.0.0.');
            }
        }

        if ($this->ssl || $this->clientAuth) {
            $this->stop();
            $this->run('updateconf',
                'client_encryption_options.enabled: false',
                'client_encryption_options.require_client_auth: false'
            );
            $this->ssl        = false;
            $this->clientAuth = false;
        }
    }

    public function setupSSL()
    {
        if (!$this->ssl) {
            $this->setup(1, 0);
            $this->stop();
            $this->run('updateconf',
                'client_encryption_options.enabled: true',
                'client_encryption_options.keystore: ' . realpath(__DIR__ . '/ssl/.keystore'),
                'client_encryption_options.keystore_password: php-driver'
            );
            $this->ssl = true;
        }
    }

    public function setupClientVerification()
    {
        if (!$this->clientAuth) {
            $this->setup(1, 0);
            $this->stop();
            $this->run('updateconf',
                'client_encryption_options.enabled: true',
                'client_encryption_options.keystore: ' . realpath(__DIR__ . '/ssl/.keystore'),
                'client_encryption_options.keystore_password: php-driver',
                'client_encryption_options.require_client_auth: true',
                'client_encryption_options.truststore: ' . realpath(__DIR__ . '/ssl/.truststore'),
                'client_encryption_options.truststore_password: php-driver'
            );
            $this->clientAuth = true;
        }
    }

    public function enableTracing($isEnabled)
    {
        $nodes = $this->dataCenterOneNodes + $this->dataCenterTwoNodes;
        for($node = 1; $node <= $nodes; ++$node) {
            $this->run('node'.$node, 'nodetool', 'settraceprobability', ((bool) $isEnabled) ? 1 : 0);
        }
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
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN' || strtoupper(substr(PHP_OS, 0, 6)) === 'CYGWIN') {
            $keepWindowsContext = '';
            if ($args[0] != "\"start\"") {
                $keepWindowsContext = '/B ';
            }
            $command = 'START "PHP Integration Tests" ' . $keepWindowsContext . '/MIN /WAIT ' . $command;
        }
        $this->process->setCommandLine($command);

        echo 'ccm > ' . $command . "\n";
        $this->process->mustRun(function ($type, $buffer) {
            echo 'ccm > ' . $buffer;
        });

        return $this->process->getOutput();
    }

    public function removeCluster($cluster)
    {
      return $this->run('remove', $cluster);
    }

    public function removeAllClusters()
    {
        $clusters = $this->getClusters();
        foreach ($clusters['list'] as $cluster) {
            $this->removeCluster($cluster);
        }
    }
}
