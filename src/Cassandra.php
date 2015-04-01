<?php

/**
 * Copyright 2015 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

use Cassandra\Cluster\Builder as ClusterBuilder;
use Cassandra\SSLOptions\Builder as SSLOptionsBuilder;

/**
 * The main entry point to the PHP Driver for Apache Cassandra
 *
 * Use Cassandra::cluster() to build a cluster instance.
 * Use Cassandra::ssl() to build SSL options instance.
 */
final class Cassandra
{
    const CONSISTENCY_ANY          = 0;
    const CONSISTENCY_ONE          = 1;
    const CONSISTENCY_TWO          = 2;
    const CONSISTENCY_THREE        = 3;
    const CONSISTENCY_QUORUM       = 4;
    const CONSISTENCY_ALL          = 5;
    const CONSISTENCY_LOCAL_QUORUM = 6;
    const CONSISTENCY_EACH_QUORUM  = 7;
    const CONSISTENCY_SERIAL       = 8;
    const CONSISTENCY_LOCAL_SERIAL = 9;
    const CONSISTENCY_LOCAL_ONE    = 10;

    const VERIFY_NONE          = 0;
    const VERIFY_PEER_CERT     = 1;
    const VERIFY_PEER_IDENTITY = 2;

    const BATCH_LOGGED   = 0;
    const BATCH_UNLOGGED = 1;
    const BATCH_COUNTER  = 2;

    const LOG_DISABLED = 0;
    const LOG_CRITICAL = 1;
    const LOG_ERROR    = 2;
    const LOG_WARN     = 3;
    const LOG_INFO     = 4;
    const LOG_DEBUG    = 5;
    const LOG_TRACE    = 6;

    const TYPE_TEXT      = "text";
    const TYPE_ASCII     = "ascii";
    const TYPE_VARCHAR   = "varchar";
    const TYPE_BIGINT    = "bigint";
    const TYPE_BLOB      = "blob";
    const TYPE_BOOLEAN   = "boolean";
    const TYPE_COUNTER   = "counter";
    const TYPE_DECIMAL   = "decimal";
    const TYPE_DOUBLE    = "double";
    const TYPE_FLOAT     = "float";
    const TYPE_INT       = "int";
    const TYPE_TIMESTAMP = "timestamp";
    const TYPE_UUID      = "uuid";
    const TYPE_VARINT    = "varint";
    const TYPE_TIMEUUID  = "timeuuid";
    const TYPE_INET      = "inet";

    const LOAD_BALANCING_ROUND_ROBIN          = 0;
    const LOAD_BALANCING_DC_AWARE_ROUND_ROBIN = 1;

    const VERSION = '1.0.0-alpha';

    /**
     * Returns a Cluster Builder
     *
     * @return Cassandra\Cluster\Builder a Cluster Builder instance
     */
    public static function cluster()
    {
        return new ClusterBuilder();
    }

    /**
     * Returns SSL Options Builder
     *
     * @return Cassanrda\SSLOptions\Builder a SSLOptions Builder instance
     */
    public static function ssl()
    {
        return new SSLOptionsBuilder();
    }

    /**
     * Sets log severity
     *
     * @param  int  $level  log severity, must be one of Cassandra::LOG_* constants.
     * @return void
     */
    public static function setLogLevel($level)
    {
        cassandra_set_log_level($level);
    }
}
