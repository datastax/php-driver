<?php

namespace Cassandra

final class Varint
{
    private $value;

    public function __construct($value);
    public function __toString();
    public function value();
}
