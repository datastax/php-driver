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

namespace Cassandra\Type;

/**
 * A class that represents a custom type.
 */
final class Custom extends Type {

    private function __construct() { }

    /**
     * {@inheritDoc}
     *
     * @return string The name of this type
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return string String representation of this type
     */
    public function __toString() { }

    /**
     * @param mixed $value
     * @return mixed
     */
    public function create($value) { }

}
