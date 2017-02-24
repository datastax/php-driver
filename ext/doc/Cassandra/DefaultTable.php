<?php

/**
 * Copyright 2017 DataStax, Inc.
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

namespace Cassandra;

/**
 * A PHP representation of a table
 */
final class DefaultTable implements Table {

    /**
     * {@inheritDoc}
     *
     * @return string Name of the table
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     *
     * @return Value Value of an option by name
     */
    public function option($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array A dictionary of `string` and `Value` pairs of the
     */
    public function options() { }

    /**
     * {@inheritDoc}
     *
     * @return string Table description or null
     */
    public function comment() { }

    /**
     * {@inheritDoc}
     *
     * @return float Read repair chance
     */
    public function readRepairChance() { }

    /**
     * {@inheritDoc}
     *
     * @return float Local read repair chance
     */
    public function localReadRepairChance() { }

    /**
     * {@inheritDoc}
     *
     * @return int GC grace seconds
     */
    public function gcGraceSeconds() { }

    /**
     * {@inheritDoc}
     *
     * @return string Caching options
     */
    public function caching() { }

    /**
     * {@inheritDoc}
     *
     * @return float Bloom filter FP chance
     */
    public function bloomFilterFPChance() { }

    /**
     * {@inheritDoc}
     *
     * @return int Memtable flush period in milliseconds
     */
    public function memtableFlushPeriodMs() { }

    /**
     * {@inheritDoc}
     *
     * @return int Default TTL.
     */
    public function defaultTTL() { }

    /**
     * {@inheritDoc}
     *
     * @return string Speculative retry.
     */
    public function speculativeRetry() { }

    /**
     * {@inheritDoc}
     *
     * @return int Index interval
     */
    public function indexInterval() { }

    /**
     * {@inheritDoc}
     *
     * @return string Compaction strategy class name
     */
    public function compactionStrategyClassName() { }

    /**
     * {@inheritDoc}
     *
     * @return Map Compaction strategy options
     */
    public function compactionStrategyOptions() { }

    /**
     * {@inheritDoc}
     *
     * @return Map Compression parameters
     */
    public function compressionParameters() { }

    /**
     * {@inheritDoc}
     *
     * @return bool Value of `populate_io_cache_on_flush` or null
     */
    public function populateIOCacheOnFlush() { }

    /**
     * {@inheritDoc}
     *
     * @return bool Value of `replicate_on_write` or null
     */
    public function replicateOnWrite() { }

    /**
     * {@inheritDoc}
     *
     * @return int Value of `max_index_interval` or null
     */
    public function maxIndexInterval() { }

    /**
     * {@inheritDoc}
     *
     * @return int Value of `min_index_interval` or null
     */
    public function minIndexInterval() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name Name of the column
     *
     * @return Column Column instance
     */
    public function column($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array A list of `Column` instances
     */
    public function columns() { }

    /**
     * {@inheritDoc}
     *
     * @return array A list of of `Column` instance
     */
    public function partitionKey() { }

    /**
     * {@inheritDoc}
     *
     * @return array A list of of `Column` instance
     */
    public function primaryKey() { }

    /**
     * {@inheritDoc}
     *
     * @return array A list of of `Column` instances
     */
    public function clusteringKey() { }

    /**
     * {@inheritDoc}
     *
     * @return array A list of cluster column orders ('asc' and 'desc')
     */
    public function clusteringOrder() { }

    /**
     * Get an index by name
     *
     * @param string $name Index name
     *
     * @return Index|null An index or null
     */
    public function index($name) { }

    /**
     * Gets all indexes
     *
     * @return array An array of indexes
     */
    public function indexes() { }

    /**
     * Get materialized view by name
     *
     * @param string $name Materialized view name
     *
     * @return MaterizedView|null A materialized view or null
     */
    public function materializedView($name) { }

    /**
     * Gets all materialized views
     *
     * @return array An array of materialized views
     */
    public function materializedViews() { }

}
