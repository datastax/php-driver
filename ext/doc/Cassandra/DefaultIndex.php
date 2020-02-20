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
 * A PHP representation of an index
 */
final class DefaultIndex implements Index {

    /**
     * Returns the name of the index
     *
     * @return string Name of the index
     */
    public function name() { }

    /**
     * Returns the kind of index
     *
     * @return string Kind of the index
     */
    public function kind() { }

    /**
     * Returns the target column of the index
     *
     * @return string Target column name of the index
     */
    public function target() { }

    /**
     * Return a column's option by name
     *
     * @param string $name The name of the option
     *
     * @return \Cassandra\Value Value of an option by name
     */
    public function option($name) { }

    /**
     * Returns all the index's options
     *
     * @return array A dictionary of `string` and `Value` pairs of the index's options.
     */
    public function options() { }

    /**
     * Returns the class name of the index
     *
     * @return string Class name of a custom index
     */
    public function className() { }

    /**
     * Determines if the index is a custom index.
     *
     * @return bool true if a custom index
     */
    public function isCustom() { }

}
