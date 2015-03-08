<?php

namespace Cassandra;

final class Bigint
{
    private $value;

    public function __construct($value);
    public function value();
    public function __toString();
}
