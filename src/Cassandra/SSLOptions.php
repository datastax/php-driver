<?php

namespace Cassandra;

/**
 * SSL options for Cassandra\Cluster.
 * @see Cassandra\SSLOptions\Builder
 */
final class SSLOptions
{
  /**
   * Trusted certificate paths. This is used to verify node's identity.
   * @var array|null
   */
  public $trustedCerts = null;
  /**
   * Client cert
   * @var string|null
   */
  public $clientCert = null;
  /**
   * Path to private key, this is used to verify client's identity.
   * @var string|null
   */
  public $privateKey = null;
  /**
   * Optional passphrase for the private key
   * @var string|null
   */
  public $passphrase = null;
  /**
   * A bitmask of Cassandra::VERIFY_* constants
   * @var int|null
   */
  public $verifyFlags = null;
}
