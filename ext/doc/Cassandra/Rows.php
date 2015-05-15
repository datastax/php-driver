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
 * Rows represent a result of statement execution.
 * @author Bulat Shakirzyanov
 * @author Bob McCarthy
 */
final class Rows implements \Iterator, \Countable, \ArrayAccess
{
    /**
     * @retval int number of rows
     * @see Countable::count()
     */
    public function count() {}

    /**
     * Resets the rows iterator.
     * @retval void
     * @see Iterator::rewind()
     */
    public function rewind() {}

    /**
     * Returns current row.
     *
     * @retval array current row
     * @see Iterator::current()
     */
    public function current() {}

    /**
     * Returns current index.
     *
     * @retval int index
     * @see Iterator::key()
     */
    public function key() {}

    /**
     * Advances the rows iterator by one.
     * @retval void
     * @see Iterator::next()
     */
    public function next() {}

    /**
     * @retval bool whether there are more rows available for iteration
     * @see Iterator::valid()
     */
    public function valid() {}

    /**
     * @param int $offset
     * @retval bool whether a row at a given index exists
     * @see ArrayAccess::offsetExists()
     */
    public function offsetExists(int $offset) {}

    /**
     * @param int $offset
     * @retval array|null row at a given index
     * @see ArrayAccess::offsetGet()
     */
    public function offsetGet(int $offset) {}

    /**
     * @param int   $offset
     * @param array $value
     * @retval void
     * @throws Cassandra::Exception::DomainException
     * @see ArrayAccess::offsetSet()
     */
    public function offsetSet(int $offset, array $value) {}

    /**
     * @param int $offset
     * @retval void
     * @throws Cassandra::Exception::DomainException
     * @see ArrayAccess::offsetUnset()
     */
    public function offsetUnset(int $offset) {}

    /**
     * @retval bool whether this is the last page or not
     */
    public function isLastPage() {}

    /**
     * @param float|null $timeout
     *
     * @retval Rows|null loads and returns next result page
     */
    public function nextPage(float $timeout = null) {}

    /**
     * @retval Future returns future of the next result page
     */
    public function nextPageAsync() {}

    /**
     * @retval array|null returns first row if any
     */
    public function first() {}
}
