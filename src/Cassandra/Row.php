<?php

namespace Cassandra;

final class Row implements \Iterator, \Countable, \ArrayAccess
{
    private $data;

    public function __construct(array $data)
    {
        $this->data  = $data;
    }

    public function count()
    {
        return count($this->data);
    }

    public function rewind() {
        reset($this->data);
    }

    public function current() {
        return current($this->data);
    }

    public function key() {
        return key($this->data);
    }

    public function next() {
        next($this->data);
    }

    public function valid() {
        return key($this->data) !== null;
    }

    public function offsetSet($offset, $value) {
        throw new DomainException(sprintf(
            "Cannot overwrite value at offset: %s with value: %s, rows are immutable",
            var_export($offset, true), var_export($value, true)
        ));
    }

    public function offsetUnset($offset) {
        throw new DomainException(sprintf(
            "Cannot delete value at offset: %s, rows are immutable",
            var_export($offset, true)
        ));
    }

    public function offsetExists($offset) {
        return isset($this->data[$offset]);
    }

    public function offsetGet($offset) {
        return isset($this->data[$offset]) ? $this->data[$offset] : null;
    }
}
