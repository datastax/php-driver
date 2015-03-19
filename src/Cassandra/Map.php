<?php

namespace Cassandra

final class Map implements \Countable, \IteratorAggregate, \ArrayAccess
{
    public function __construct($keyType, $valueType);
    public function keyType();
    public function keys();
    public function valueType();
    public function values();
    public function set($key, $value);
    public function get($key);
    public function remove($key);
    public function has($key);
}
