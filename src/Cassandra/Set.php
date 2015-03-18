<?php

namespace Cassandra

final class Set implements \Countable, \IteratorAggregate
{
    public static function of(Type $type);
    public function __toString();
    public function type();
    public function toArray();
    public function add($value);
    public function remove($value);
}

new Cassandra\Set('123', '456', '789');
Cassandra\Set(Cassandra\Varchar())->create('123', '456', '789');
