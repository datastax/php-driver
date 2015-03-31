<?php

namespace Cassandra

/**
 * A PHP representation of the CQL `set` datatype
 */
final class Set implements \Countable, \IteratorAggregate
{
    /**
     * Creates a new collection of a given type
     * @param string $type one of `Cassandra::TYPE_*`
     */
    public function __construct($type) {}

    /**
     * The type of this set
     * @return string one of `Cassandra::TYPE_*`
     */
    public function type() {}

    /**
     * Array of values in this set
     * @return array values
     */
    public function values() {}

    /**
     * Adds a value to this set
     * @param  mixed   $value Value
     * @return boolean whether the value has been added
     */
    public function add($value) {}

    /**
     * Removes a value to this set
     * @param  mixed   $value Value
     * @return boolean whether the value has been removed
     */
    public function remove($value) {}

    /**
     * Returns whether a value is in this set
     * @param  mixed   $value Value
     * @return boolean whether the value is in the set
     */
    public function has($value) {}
}
