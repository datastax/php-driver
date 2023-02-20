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
 * A PHP representation of a column
 */
final class DefaultColumn implements Column {

    /**
     * Returns the name of the column.
     *
     * @return string Name of the column or null
     */
    public function name() { }

    /**
     * Returns the type of the column.
     *
     * @return \Cassandra\Type Type of the column
     */
    public function type() { }

    /**
     * Returns whether the column is in descending or ascending order.
     *
     * @return bool Whether the column is stored in descending order.
     */
    public function isReversed() { }

    /**
     * Returns true for static columns.
     *
     * @return bool Whether the column is static
     */
    public function isStatic() { }

    /**
     * Returns true for frozen columns.
     *
     * @return bool Whether the column is frozen
     */
    public function isFrozen() { }

    /**
     * Returns name of the index if defined.
     *
     * @return string Name of the index if defined or null
     */
    public function indexName() { }

    /**
     * Returns index options if present.
     *
     * @return string Index options if present or null
     */
    public function indexOptions() { }

}
