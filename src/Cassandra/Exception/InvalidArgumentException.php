<?php

namespace Cassandra\Exception;

use Cassandra\Exception;

/**
 * Cassandra-specific invalid argument exception
 */
class InvalidArgumentException extends \InvalidArgumentException implements Exception
{
}
