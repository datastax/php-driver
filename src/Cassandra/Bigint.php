<?php

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;

final class Bigint
{
    public function __construct($value);

    public function value();

    public function __toString();
}
