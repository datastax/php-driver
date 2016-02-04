<?php

namespace Cassandra\Type;

use Cassandra\Type;

class UnsupportedType implements Type
{
    public function name()
    {
        return "unsupported";
    }

    public function __toString()
    {
        return "unsupported";
    }

    public function create($value = null)
    {
        return null;
    }
}
