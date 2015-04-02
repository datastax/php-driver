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
 * Rows represent a result of statement execution.
 */
final class Rows implements \Iterator, \Countable, \ArrayAccess
{
    /**
     * Result resource, used for paging
     * @var resource
     */
    private $resource;

    /**
     * Session resource, used for paging
     * @var resource
     */
    private $session;

    /**
     * Statement resource, used for paging
     * @var resource
     */
    private $statement;

    /**
     * Actual rows contained in this page of the result
     * @var array
     */
    private $rows;

    /**
     * Creates a new instance of rows.
     * @access private
     * @param resource $resource  Result resource, used for paging
     * @param resource $session   Session resource, used for paging
     * @param resource $statement Statement resource, used for paging
     */
    public function __construct($resource, $session, $statement)
    {
        $this->resource  = $resource;
        $this->session   = $session;
        $this->statement = $statement;
        $this->rows      = array();

        foreach (cassandra_rows_from_result($this->resource) as $row) {
            $this->rows[] = new Row($row);
        }
    }

    /**
     * {@inheritDoc}
     */
    public function count()
    {
        return count($this->rows);
    }

    /**
     * Resets the rows iterator
     * @return void
     */
    public function rewind()
    {
        reset($this->rows);
    }

    /**
     * Returns current row
     * @return mixed value
     */
    public function current()
    {
        return current($this->rows);
    }

    /**
     * Returns current index
     * @return string name
     */
    public function key()
    {
        return key($this->rows);
    }

    /**
     * Advances the rows iterator by one
     * @return void
     */
    public function next()
    {
        next($this->rows);
    }

    /**
     * @return boolean whether there are more rows available for iteration
     */
    public function valid()
    {
        return key($this->rows) !== null;
    }

    /**
     * {@inheritDoc}
     */
    public function offsetExists($offset)
    {
        if (!is_int($offset)) {
            throw new DomainException(sprintf(
                "Unsupported offset: %s, offset must be an integer",
                var_export($offset, true)
            ));
        }

        return $offset >= 0 && $offset < $this->count;
    }

    /**
     * {@inheritDoc}
     */
    public function offsetGet($offset)
    {
        if (!$this->offsetExists($offset)) {
            return null;
        }

        return $this->rows[$offset];
    }

    /**
     * {@inheritDoc}
     */
    public function offsetSet($offset, $value)
    {
        throw new DomainException(sprintf(
            "Cannot overwrite row at offset: %s with value: %s, results are immutable",
            var_export($offset, true), var_export($value, true)
        ));
    }

    /**
     * {@inheritDoc}
     */
    public function offsetUnset($offset)
    {
        throw new DomainException(sprintf(
            "Cannot delete row at offset: %s, results are immutable",
            var_export($offset, true)
        ));
    }

    /**
     * @return  boolean  whether this is the last page or not
     */
    public function isLastPage()
    {
        return !cassandra_result_has_more_pages($this->resource);
    }

    /**
     * @return  Cassandra\Result|null  loads and returns next result page
     */
    public function nextPage($timeout = null)
    {
        return $this->nextPageAsync()->get($timeout);
    }

    /**
     * @return  Cassandra\Future  returns future of the next result page
     */
    public function nextPageAsync()
    {
        try {
            cassandra_statement_set_paging_state($this->statement, $this->resource);
        } catch (Exception $e) {
            return new FutureException($e);
        }

        return new FutureRows(
            cassandra_session_execute($this->session, $this->statement),
            $this->session, $this->statement
        );
    }

    /**
     * @return  Cassandra\Row|null  returns first row if any
     */
    public function first()
    {
        if (isset($this->rows[0])) {
            return $this->rows[0];
        }
    }
}
