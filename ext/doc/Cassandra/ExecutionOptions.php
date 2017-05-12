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
 * Request execution options.
 *
 * @deprecated Use an array of options instead of creating an instance of this class.
 *
 * @see Session::execute()
 * @see Session::executeAsync()
 * @see Session::prepare()
 * @see Session::prepareAsync()
 */
final class ExecutionOptions {

    /**
     * Creates a new options object for execution.
     *
     * @param array $options various execution options
     *
     * @throws Exception\InvalidArgumentException
     *
     * @see Session::execute() for valid execution options
     */
    public function __construct($options) { }

    /**
     * @param mixed $name
     * @return mixed
     */
    public function __get($name) { }

}
