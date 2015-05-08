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
 * SSLOptions builder allows fluent configuration of ssl options.
 *
 * @see Cassandra::ssl()
 * @see Cassandra\Cluster\Builder::withSSL()
 */
final class Builder
{
    /**
     * A list of paths to trusted certificates (server certs).
     *
     * @var array|null
     */
    private $trustedCerts;

    /**
     * Verification flags, a bitwise combination of one or more values of `Cassandra::VERIFY_*`.
     *
     * @var int|null
     */
    private $verifyFlags;

    /**
     * Path to client cert for client ssl authentication.
     *
     * @var string|null
     */
    private $clientCert;

    /**
     * Path to private key for client ssl authentication.
     *
     * @var string|null
     */
    private $privateKey;

    /**
     * Private key passphrase if any.
     *
     * @var string|null
     */
    private $passphrase;

    /**
     * Adds a trusted certificate. This is used to verify node's identity.
     *
     * @param string $path path to a file containing a PEM formatted certificate.
     * @param string ...   additional paths
     *
     * @return Builder self
     */
    public function withTrustedCerts($path) {}

    /**
     * Disable certificate verification.
     *
     * @param int $flags
     *
     * @return self
     */
    public function withVerifyFlags($flags) {}
    /**
     * Set client-side certificate chain.
     *
     * This is used to authenticate the client on the server-side. This should contain the entire Certificate
     * chain starting with the certificate itself.
     *
     * @param string $path path to a file containing a PEM formatted certificate.
     *
     * @return self
     */
    public function withClientCert($path) {}
    /**
     * Set client-side private key. This is used to authenticate the client on
     * the server-side.
     *
     * @param string      $path       Path to the private key file
     * @param string|null $passphrase Passphrase for the private key, if any (default: `null`)
     *
     * @return self
     */
    public function withPrivateKey($path, $passphrase = null) {}
    /**
     * Builds SSL options.
     *
     * @return SSLOptions ssl options configured accordingly.
     */
    public function build() {}
}
