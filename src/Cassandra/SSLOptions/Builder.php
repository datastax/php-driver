<?php

/**
 * Copyright 2015 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace Cassandra\SSLOptions;

use Cassandra\SSLOptions;
use Cassandra\Exception\InvalidArgumentException;

/**
 * SSLOptions builder allows fluent configuration of ssl options
 * @see Cassandra::ssl()
 * @see Cassandra\Cluster\Builder::withSSL()
 */
final class Builder
{
    /**
     * A list of paths to trusted certificates (server certs)
     * @var array|null
     */
    private $trustedCerts;

    /**
     * Verification flags, a bitwise combination of one or more values of `Cassandra::VERIFY_*`
     * @var int|null
     */
    private $verifyFlags;

    /**
     * Path to client cert for client ssl authentication.
     * @var string|null
     */
    private $clientCert;

    /**
     * Path to private key for client ssl authentication.
     * @var string|null
     */
    private $privateKey;

    /**
     * Private key passphrase if any.
     * @var string|null
     */
    private $passphrase;

    /**
     * Creates new SSL Options builder
     */
    public function __construct()
    {
        $this->trustedCerts = null;
        $this->verifyFlags  = null;
        $this->clientCert   = null;
        $this->privateKey   = null;
        $this->passphrase   = null;
    }

    /**
     * Adds a trusted certificate. This is used to verify node's identity.
     *
     * @param string $path path to a file containing a PEM formatted certificate.
     * @param string ...   additional paths
     *
     * @return Cassandra\SSLOptions\Builder self
     */
    public function withTrustedCerts($path)
    {
        $paths = func_get_args();
        foreach ($paths as $path) {
            if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
                throw new InvalidArgumentException(sprintf(
                    "Trusted cert path '%s' doesn't exist or is not readable",
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
     * Set client-side certificate chain. This is used to authenticate the
     * client on the server-side. This should contain the entire Certificate
     * chain starting with the certificate itself.
     *
     * @param [type] $path path to a file containing a PEM formatted certificate.
     *
     * @return Cassandra\SSLOptions\Builder self
     */
    public function withClientCert($path)
    {
        if (!file_exists($path) || !is_file($path) || !is_readable($path)) {
            throw new InvalidArgumentException(sprintf(
                "Client cert path '%s' doesn't exist or is not readable",
                $path
            ));
        }
        $this->clientCert = $path;
        return $this;
    }

    /**
     * Set client-side private key. This is used to authenticate the client on
     * the server-side.
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
                "Client cert path '%s' doesn't exist or is not readable",
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

        if (!is_null($this->trustedCerts)) {
            $options->trustedCerts = $this->trustedCerts;
        }
        if (!is_null($this->verifyFlags)) {
            $options->verifyFlags = $this->verifyFlags;
        }
        if (!is_null($this->clientCert)) {
            $options->clientCert = $this->clientCert;
        }
        if (!is_null($this->privateKey)) {
            $options->privateKey = $this->privateKey;
        }
        if (!is_null($this->passphrase)) {
            $options->passphrase = $this->passphrase;
        }

        return $options;
    }
}
