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
 * Default cluster implementation.
 *
 * @see Cluster
 */
final class DefaultCluster implements Cluster {

    /**
     * Creates a new Session instance.
     *
     * @param string $keyspace Optional keyspace name
     * @param int $timeout Optional timeout
     *
     * @return \Cassandra\Session Session instance
     */
    public function connect($keyspace, $timeout) { }

    /**
     * Creates a new Session instance.
     *
     * @param string $keyspace Optional keyspace name
     *
     * @return \Cassandra\Future A Future Session instance
     */
    public function connectAsync($keyspace) { }

}
