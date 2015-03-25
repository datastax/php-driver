<?php

namespace Cassandra;

use Cassandra\Exception\DomainException;

/**
 * Rows represent a result of statement execution.
 */
final class Rows implements \Iterator, \Countable, \ArrayAccess
{
    /**
     * @access private
     */
    private $rows;

    /**
     * @access private
     */
    public function __construct(array $rows)
    {
        $this->rows = $rows;
    }

    /**
     * {@inheritDoc}
     */
    public function count()
    {
        return count($this->rows);
    }

    /**
     * Resets the values iterator
     * @return void
     */
    public function rewind()
    {
        reset($this->rows);
    }

    /**
     * Returns current column's value
     * @return mixed value
     */
    public function current()
    {
        return current($this->rows);
    }

    /**
     * Returns current column's name
     * @return string name
     */
    public function key()
    {
        return key($this->rows);
    }

    /**
     * Advances the values iterator by one
     * @return void
     */
    public function next()
    {
        next($this->rows);
    }

    /**
     * @return boolean whether there are more values available for iteration
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
    }

    /**
     * @return  Cassandra\Result|null  loads and returns next result page
     */
    public function nextPage()
    {
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
