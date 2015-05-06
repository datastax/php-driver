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
 * An individual row representation.
 */
final class Row implements \Iterator, \Countable, \ArrayAccess
{
    /**
     * Number of columns in this row.
     *
     * @return int number of columns
     * @see Countable::count()
     */
    public function count() {}

    /**
     * Resets the values iterator.
     * @return void
     * @see Iterator::rewind()
     */
    public function rewind() {}

    /**
     * Returns current column's value.
     *
     * @return mixed value
     * @see Iterator::current()
     */
    public function current() {}

    /**
     * Returns current column's name.
     *
     * @return string name
     * @see Iterator::key()
     */
    public function key() {}

    /**
     * Advances the values iterator by one.
     * @return void
     * @see Iterator::next()
     */
    public function next() {}

    /**
     * @return bool whether there are more values available for iteration
     * @see Iterator::valid()
     */
    public function valid() {}

    /**
     * Rows are read-only so this method throws an exception and doesn't do anything.
     *
     * @throws DomainException
     *
     * @param string $offset
     * @param mixed  $value
     * @return void
     * @see ArrayAccess::offsetSet()
     */
    public function offsetSet($offset, $value) {}

    /**
     * Rows are read-only so this method throws an exception and doesn't do anything.
     *
     * @throws DomainException
     *
     * @param string $offset
     * @return void
     * @see ArrayAccess::offsetUnset()
     */
    public function offsetUnset($offset) {}

    /**
     * Checks if a column is present in this row.
     *
     * @param string $offset column name
     *
     * @return bool
     * @see ArrayAccess::offsetExists()
     */
    public function offsetExists($offset) {}

    /**
     * Gets the value of a given column.
     *
     * @param string $offset column name
     *
     * @return mixed|null value
     * @see ArrayAccess::offsetGet()
     */
    public function offsetGet($offset) {}
}
