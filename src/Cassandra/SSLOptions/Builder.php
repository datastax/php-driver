<?php

namespace Cassandra\SSLOptions;

use Cassandra\SSLOptions;
use Cassandra\Exception\InvalidArgumentException;

final class Builder
{
    private $trustedCerts;
    private $verifyFlags;
    private $clientCert;
    private $privateKey;
    private $passphrase;

    public function __construct()
    {
        $this->verifyFlags = 0;
    }

    /**
     * Adds a trusted certificate. This is used to verify node's identity.
     *
     * @param string $path path to a file containing a PEM formatted certificate.
     *
     * @return Cassandra\SSLOptions\Builder self
     */
    public function withTrustedCert(array $paths)
    {
        foreach ($paths as $path) {
            if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
                throw new InvalidArgumentException(sprintf(
                    "Trusted cert path %s doesn't exist or is not readable",
                    $path
                ));
            }
        }
        $this->trustedCerts = $paths;
        return $this;
    }

    /**
     * Disable certificate verification
     *
     * @return Cassandra\SSLOptions\Builder self
     */
    public function withVerifyFlags($flags)
    {
        $this->verifyFlags = (int) $flags;
        return $this;
    }

    /**
     * Set client-side certificate chain. This is used to authenticate the client on the server-side. This should contain the entire Certificate chain starting with the certificate itself.
     *
     * @param [type] $path path to a file containing a PEM formatted certificate.
     *
     * @return Cassandra\SSLOptions\Builder self
     */
    public function withClientCert($path)
    {
        if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
            throw new InvalidArgumentException(sprintf(
                "Client cert path %s doesn't exist or is not readable",
                $path
            ));
        }
        $this->clientCert = $path;
        return $this;
    }

    /**
     * Set client-side private key. This is used to authenticate the client on the server-side.
     *
     * @param string $path       Path to the private key file
     * @param string $passphrase Passphrase for the private key, if any (default: `null`)
     *
     * @return Cassandra\SSLOptions\Builder self
     */
    function withPrivateKey($path, $passphrase = null)
    {
        if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
            throw new InvalidArgumentException(sprintf(
                "Client cert path %s doesn't exist or is not readable",
                $path
            ));
        }
        $this->privateKey = $path;

        if (!is_null($passphrase)) {
            $this->passphrase = (string) $passphrase;
        }

        return $this;
    }

    /**
     * Builds SSL options
     * @return Cassandra\SSLOptions ssl options configured accordingly.
     */
    public function build()
    {
        $options = new SSLOptions();
    }
}
