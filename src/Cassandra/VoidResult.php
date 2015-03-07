<?php

namespace Cassandra;

final class VoidResult implements Result
{
    private $iterator;

    public function __construct()
    {
        $this->iterator = new \EmptyIterator();
    }

    /**
     * {@inheritDoc}
     */
    public function count()
    {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public function offsetExists($offset)
    {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public function offsetGet($offset)
    {
    }

    /**
     * {@inheritDoc}
     */
    public function offsetSet($offset, $value)
    {
        throw new DomainException(sprintf(
            "Cannot overwrite rows in the result at offset: %s with value: %s, results are immutable",
            var_export($offset, true), var_export($value, true)
        ));
    }

    /**
     * {@inheritDoc}
     */
    public function offsetUnset($offset)
    {
        throw new DomainException(sprintf(
            "Cannot delete rows at offset: %s, results are immutable",
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
     * {@inheritDoc}
     */
    public function isLastPage()
    {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public function nextPage()
    {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public function first()
    {
        return null;
    }
}
