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
 * Futures are returns from asynchronous methods.
 *
 * @see Cluster::connectAsync()
 * @see Session::executeAsync()
 * @see Session::prepareAsync()
 * @see Session::closeAsync()
 */
interface Future {

    /**
     * Waits for a given future resource to resolve and throws errors if any.
     *
     * @param int|double|null $timeout A timeout in seconds
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @return mixed a value that the future has been resolved with
     */
    public function get($timeout);

}
