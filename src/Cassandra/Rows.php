<?php

namespace Cassandra;

final class Rows implements \Countable, \ArrayAccess, \IteratorAggregate
{
    private $resource;
    private $iterator;

    /**
     * @access private
     */
    public function __construct($resource)
    {
        $this->resource = $resource;
        $this->rows     = array();

        foreach (cassanrda_rows_from_result($resource) as $i => $row) {
            $this->rows[$i] = new Row($row);
        }

        $this->count    = count($this->rows);

        $this->iterator = new \ArrayIterator($this->rows);
    }

    /**
     * {@inheritDoc}
     */
    public function count()
    {
        return $this->count;
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
     * {@inheritDoc}
     */
    public function getIterator()
    {
        return $this->iterator;
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
