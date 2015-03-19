<?php

namespace Cassandra

final class Set implements \Countable, \IteratorAggregate
{
    public function __construct($type);
    public function type();
    public function values();
    public function add($value);
    public function remove($value);
    public function has($value);
}
