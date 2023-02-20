<?php

/**
 * Copyright 2017 DataStax, Inc.
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
 * A PHP representation of the CQL `duration` datatype
 */
final class Duration implements Value {

    /**
     * @param long|double|string|\Cassandra\Bigint $months Months attribute of the duration.
     * @param long|double|string|\Cassandra\Bigint $days Days attribute of the duration.
     * @param long|double|string|\Cassandra\Bigint $nanos Nanos attribute of the duration.
     */
    public function __construct($months, $days, $nanos) { }

    /**
     * The type of represented by the value.
     *
     * @return \Cassandra\Type the Cassandra type for Duration
     */
    public function type() { }

    /**
     * @return string the months attribute of this Duration
     */
    public function months() { }

    /**
     * @return string the days attribute of this Duration
     */
    public function days() { }

    /**
     * @return string the nanoseconds attribute of this Duration
     */
    public function nanos() { }

    /**
     * @return string string representation of this Duration; may be used as a literal parameter in CQL queries.
     */
    public function __toString() { }

}
