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

use Iterator;
use Countable;
use ArrayAccess;

/**
 * Rows represent a result of statement execution.
 */
final class Rows implements Iterator, Countable, ArrayAccess
{
    /**
     * @return int number of rows
     * @see Countable::count()
     */
    public function int count() {}

    /**
     * Resets the rows iterator.
     * @return void
     * @see Iterator::rewind()
     */
    public function void rewind() {}

    /**
     * Returns current row.
     *
     * @return array current row
     * @see Iterator::current()
     */
    public function array current() {}

    /**
     * Returns current index.
     *
     * @return int index
     * @see Iterator::key()
     */
    public function int key() {}

    /**
     * Advances the rows iterator by one.
     * @return void
     * @see Iterator::next()
     */
    public function void next() {}

    /**
     * @return bool whether there are more rows available for iteration
     * @see Iterator::valid()
     */
    public function bool valid() {}

    /**
     * @param int $offset
     * @return bool whether a row at a given index exists
     * @see ArrayAccess::offsetExists()
     */
    public function bool offsetExists(int $offset) {}

    /**
     * @param int $offset
     * @return array|null row at a given index
     * @see ArrayAccess::offsetGet()
     */
    public function array|null offsetGet(int $offset) {}

    /**
     * @param int   $offset
     * @param array $value
     * @return void
     * @throws Cassandra::Exception::DomainException
     * @see ArrayAccess::offsetSet()
     */
    public function void offsetSet(int $offset, array $value) {}

    /**
     * @param int $offset
     * @return void
     * @throws Cassandra::Exception::DomainException
     * @see ArrayAccess::offsetUnset()
     */
    public function void offsetUnset(int $offset) {}

    /**
     * @return bool whether this is the last page or not
     */
    public function bool isLastPage() {}

    /**
     * @param float|null $timeout
     *
     * @return Rows|null loads and returns next result page
     */
    public function Rows|null nextPage(float $timeout = null) {}

    /**
     * @return Future returns future of the next result page
     */
    public function Future nextPageAsync() {}

    /**
     * @return array|null returns first row if any
     */
    public function array|null first() {}
}
