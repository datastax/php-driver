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
