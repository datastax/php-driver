<?php

namespace Cassandra

final class Collection implements \Countable, \IteratorAggregate
{
    public function __construct($type);
    public function type();
    public function values();
    public function add($value);
    public function get($index);
    public function find($value);
}
