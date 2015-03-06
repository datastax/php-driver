<?php

namespace Cassandra\Cluster;

interface SSLContext
{
    /**
     * Adds a trusted certificate. This is used to verify node's identity.
     *
     * @param string $path path to a file containing a PEM formatted certificate.
     *
     * @return Cassandra\Cluster\SSLContext self
     */
    function addTrustedCert($path);

    /**
     * Disable certificate verification
     *
     * @return Cassandra\Cluster\SSLContext self
     */
    function verifyNone();

    /**
     * Verify that the certificate is present and valid
     *
     * @return Cassandra\Cluster\SSLContext self
     */
    function verifyCertificate();

    /**
     * Verify that the IP address matches the certificate’s common name or one of its subject alternative names. This implies the certificate is also present.
     *
     * @return Cassandra\Cluster\SSLContext self
     */
    function verifyIdentity();

    /**
     * Set client-side certificate chain. This is used to authenticate the client on the server-side. This should contain the entire Certificate chain starting with the certificate itself.
     *
     * @param [type] $path path to a file containing a PEM formatted certificate.
     *
     * @return Cassandra\Cluster\SSLContext self
     */
    function setClientCert($path);

    /**
     * Set client-side private key. This is used to authenticate the client on the server-side.
     *
     * @param string $path       Path to the private key file
     * @param string $passphrase Passphrase for the private key, if any (default: `null`)
     *
     * @return Cassandra\Cluster\SSLContext self
     */
    function setPrivateKey($path, $passphrase = null);

    /**
     * @access private
     *
     * @return resource  Actual ssl context resource
     */
    function resource();
}
