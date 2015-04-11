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
 * Default cluster implementation.
 *
 * @see Cassandra\Cluster
 */
final class DefaultCluster implements Cluster
{
    /**
     * Cluster resource.
     *
     * @var resource
     */
    private $resource;

    /**
     * Default cluster-wide execution options.
     *
     * @var ExecutionOptions
     */
    private $defaults;

    /**
     * Holds a reference to SSLContext resource if was configured with SSL.
     * This is necessary to prevent GC from destroying it before the cluster.
     *
     * @see https://datastax-oss.atlassian.net/projects/PHP/issues/PHP-5
     *
     * @var resource|null
     */
    private $ssl;

    /**
     * The contact points for the cluster.  This is used for persistent Sessions key.
     *
     * @var string
     */
    private $contactPoints;

    /**
     * The port used to connect to the cluster.  This is used for persistent Sessions key.
     *
     * @var int
     */
    private $port;

    /**
     * Note that while the $ssl property is not used, it is necessary to keep
     * a reference to it to prevent GC from destroying it before the cluster.
     *
     * @access private
     *
     * @param resource         $resource Cluster resource
     * @param ExecutionOptions $defaults
     * @param string           $contactPoints Used for persistent sessions.
     * @param int              $port          Used for persistent sessions.
     * @param resource         $ssl
     */
    public function __construct(
        $resource,
        ExecutionOptions $defaults,
        $contactPoints,
        $port = 9042,
        $ssl = null
    )
    {
        $this->resource      = $resource;
        $this->defaults      = $defaults;
        $this->contactPoints = $contactPoints;
        $this->port          = $port;
        $this->ssl           = $ssl;
    }

    /**
     * {@inheritDoc}
     */
    public function connect($keyspace = null)
    {
        return $this->connectAsync($keyspace)->get();
    }

    /**
     * {@inheritDoc}
     */
    public function connectAsync($keyspace = null)
    {
        $session = cassandra_session_new();
        return $this->_connect($session, $keyspace);
    }

    /**
     * {@inheritDoc}
     */
    public function pconnect($keyspace = null)
    {
        $result = $this->pconnectAsync($keyspace);

        /**
         * pconnectAsync() can return bool|Session|FutureSession,
         * handle each case accordingly.
         **/
        if (is_bool($result) && $result === false) {
            return false;
        }

        if ($result instanceof Session) {
            return $result;
        }

        return $result->get(
            isset($this->defaults->timeout)
                ? $this->defaults->timeout
                : null
        );
    }

    /**
     * {@inheritDoc}
     */
    public function pconnectAsync($keyspace = null)
    {
        /**
         * Key the persistent connection under:
         * <contact_points>:<port>:<keyspace>
         * This allows for multiple persistent connections with different
         * keyspaces to be active at a given time.
         **/
        $key = $this->contactPoints . ":" . $this->port;
        if (!is_null($keyspace)) {
            $key .= ":" . $keyspace;
        }

        /**
         * cassandra_session_pnew() will return
         * array(
         *     "found" => bool,
         *     "session" => CassSession
         * )
         *
         * If the persistent session was not found then it
         * falls back and creates a new persistent session.
         *
         * If the persistent session was found then avoid
         * re-connecting to the cassandra cluster by
         * simply returning the persistent session.
         */
        $session = cassandra_session_pnew($key);
        if (!$session["found"]) {
            return $this->_connect($session["session"], $keyspace);
        }
        return new DefaultSession($session["session"], $this->defaults);
    }

    /**
     * Helper function to connect once a Session has been obtained.
     *
     * @param resource $session CassSession resource to connect with.
     *
     * @param string $keyspace Optional keyspace.
     *
     * @return Future
     */
    private function _connect($session, $keyspace = null)
    {
        if (is_null($keyspace)) {
            $future = cassandra_session_connect($session, $this->resource);
        } else {
            $future = cassandra_session_connect_keyspace($session, $this->resource, $keyspace);
        }

        return new FutureSession($future, new DefaultSession($session, $this->defaults));
    }
}
