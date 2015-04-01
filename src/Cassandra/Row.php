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

use Cassandra\Exception\DomainException;

/**
 * An individual row representation
 */
final class Row implements \Iterator, \Countable, \ArrayAccess
{
    /**
     * Actual data
     * @var array
     */
    private $data;

    /**
     * Creates a new row.
     * @access private
     * @param array $data data for the row
     */
    public function __construct(array $data)
    {
        $this->data = $data;
    }

    /**
     * Number of columns in this row
     * @return int number of columns
     */
    public function count()
    {
        return count($this->data);
    }

    /**
     * Resets the values iterator
     * @return void
     */
    public function rewind()
    {
        reset($this->data);
    }

    /**
     * Returns current column's value
     * @return mixed value
     */
    public function current()
    {
        return current($this->data);
    }

    /**
     * Returns current column's name
     * @return string name
     */
    public function key()
    {
        return key($this->data);
    }

    /**
     * Advances the values iterator by one
     * @return void
     */
    public function next()
    {
        next($this->data);
    }

    /**
     * @return boolean whether there are more values available for iteration
     */
    public function valid()
    {
        return key($this->data) !== null;
    }

    /**
     * Rows are read-only so this method throws an exception and doesn't do anything.
     * @throws Cassandra\Exception\DomainException
     * @param string $offset
     * @param mixed  $value
     */
    public function offsetSet($offset, $value) {
        throw new DomainException(sprintf(
            "Cannot overwrite value at offset: %s with value: %s, rows are immutable",
            var_export($offset, true), var_export($value, true)
        ));
    }

    /**
     * Rows are read-only so this method throws an exception and doesn't do anything.
     * @throws Cassandra\Exception\DomainException
     * @param string $offset
     */
    public function offsetUnset($offset) {
        throw new DomainException(sprintf(
            "Cannot delete value at offset: %s, rows are immutable",
            var_export($offset, true)
        ));
    }

    /**
     * Checks if a column is present in this row
     * @param string $offset column name
     * @return boolean
     */
    public function offsetExists($offset) {
        return isset($this->data[$offset]);
    }

    /**
     * Gets the value of a given column
     * @param string $offset column name
     * @return mixed|null value
     */
    public function offsetGet($offset) {
        return isset($this->data[$offset]) ? $this->data[$offset] : null;
    }
}
