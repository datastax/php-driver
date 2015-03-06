<?php

namespace Cassandra\Cluster;

use Cassandra\Exception\InvalidArgumentException;

final class DefaultSSLContext implements SSLContext
{
  /**
   * SSL Context resource
   * @var resource
   */
  private $resource;

  public function __construct()
  {
    $this->resource = cassandra_ssl_new();
  }

  /**
   * {@inheritDoc}
   */
  public function addTrustedCert($path)
  {
    if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
      throw new InvalidArgumentException(sprintf('Path %s doesn\'t exist or is not readable', $path));
    }

    cassandra_ssl_add_trusted_cert($this->resource, file_get_contents($path));
    return $this;
  }

  /**
   * {@inheritDoc}
   */
  public function verifyNone()
  {
    cassandra_ssl_set_verify_flags($this->resource, 0);
    return $this;
  }

  /**
   * {@inheritDoc}
   */
  public function verifyCertificate()
  {
    cassandra_ssl_set_verify_flags($this->resource, 1);
    return $this;
  }

  /**
   * {@inheritDoc}
   */
  public function verifyIdentity()
  {
    cassandra_ssl_set_verify_flags($this->resource, 2);
    return $this;
  }

  /**
   * {@inheritDoc}
   */
  public function setClientCert($path)
  {
    if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
      throw new InvalidArgumentException(sprintf('Path %s doesn\'t exist or is not readable', $path));
    }

    cassandra_ssl_set_cert($this->resource, file_get_contents($path));
    return $this;
  }

  /**
   * {@inheritDoc}
   */
  public function setPrivateKey($path, $passphrase = null)
  {
    if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
      throw new InvalidArgumentException(sprintf('Path %s doesn\'t exist or is not readable', $path));
    }

    cassandra_ssl_set_private_key($this->resource, $path, $passphrase);
    return $this;
  }

  /**
   * @access private
   * @return resource|null Internal SSL Context resource
   */
  public function resource()
  {
    return $this->resource;
  }
}
