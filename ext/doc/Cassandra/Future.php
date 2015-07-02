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

namespace Cassandra;

/**
 * Futures are returns from asynchronous methods.
 *
 * @see Cluster::connectAsync()
 * @see Session::executeAsync()
 * @see Session::prepareAsync()
 * @see Session::closeAsync()
 */
interface Future
{
    /**
     * Waits for a given future resource to resolve and throws errors if any.
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @param float|null $timeout
     *
     * @return mixed a value that the future has been resolved with
     */
    public function get($timeout = null);
}
