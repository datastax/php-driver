<?php

namespace Cassandra

final class Decimal
{
    private $value;
    private $scale;

    public function __construct($value, $scale = 0);
    public function __toString();
    public function value();
    public function scale();
}
