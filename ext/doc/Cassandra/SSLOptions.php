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

namespace Cassandra;

/**
 * SSL options for Cassandra\Cluster.
 *
 * @see Cassandra\SSLOptions\Builder
 */
final class SSLOptions
{
    /**
     * Trusted certificate paths. This is used to verify node's identity.
     *
     * @var array|null
     */
    private $trustedCerts;

    /**
     * Path to client cert.
     *
     * @var string|null
     */
    private $clientCert;

    /**
     * Path to private key, this is used to verify client's identity.
     *
     * @var string|null
     */
    private $privateKey;

    /**
     * Optional passphrase for the private key.
     *
     * @var string|null
     */
    private $passphrase;

    /**
     * A bitmask of Cassandra::VERIFY_* constants.
     *
     * @var int|null
     */
    private $verifyFlags;

    /**
     * Constructs SSLOptions instance
     * Don't instantiate directly, use Cassandra\SSLOptions\Builder instead.
     *
     * @access private
     *
     * @param array|null  $trustedCerts Trusted certificate paths
     * @param int|null    $verifyFlags  A bitmask of Cassandra::VERIFY_* constants
     * @param string|null $clientCert   Path to client cert
     * @param string|null $privateKey   Path to private key
     * @param string|null $passphrase   Passphrase for the private key if any
     */
    public function __construct($trustedCerts, $verifyFlags, $clientCert, $privateKey, $passphrase)
    {
        $this->trustedCerts = $trustedCerts;
        $this->verifyFlags  = $verifyFlags;
        $this->clientCert   = $clientCert;
        $this->privateKey   = $privateKey;
        $this->passphrase   = $passphrase;
    }

    /**
     * Creates ssl context resource.
     *
     * @access private
     *
     * @return resource ssl context
     */
    public function resource()
    {
        $resource = cassandra_ssl_new();

        if (is_array($this->trustedCerts)) {
            foreach ($this->trustedCerts as $path) {
                cassandra_ssl_add_trusted_cert($resource, file_get_contents($path));
            }
        }

        if (!is_null($this->clientCert)) {
            cassandra_ssl_set_cert($resource, file_get_contents($this->clientCert));
        }

        if (!is_null($this->privateKey)) {
            cassandra_ssl_set_private_key($resource, file_get_contents($this->privateKey), $this->passphrase);
        }

        if (!is_null($this->verifyFlags)) {
            cassandra_ssl_set_verify_flags($resource, $this->verifyFlags);
        }

        return $resource;
    }
}
