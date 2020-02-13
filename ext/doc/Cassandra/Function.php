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
 * A PHP representation of a function
 */
interface Function_ {

    /**
     * Returns the full name of the function
     *
     * @return string Full name of the function including name and types
     */
    public function name();

    /**
     * Returns the simple name of the function
     *
     * @return string Simple name of the function
     */
    public function simpleName();

    /**
     * Returns the arguments of the function
     *
     * @return array Arguments of the function
     */
    public function arguments();

    /**
     * Returns the return type of the function
     *
     * @return \Cassandra\Type Return type of the function
     */
    public function returnType();

    /**
     * Returns the signature of the function
     *
     * @return string Signature of the function (same as name())
     */
    public function signature();

    /**
     * Returns the lanuage of the function
     *
     * @return string Language used by the function
     */
    public function language();

    /**
     * Returns the body of the function
     *
     * @return string Body of the function
     */
    public function body();

    /**
     * Determines if a function is called when the value is null.
     *
     * @return bool Returns whether the function is called when the input columns are null
     */
    public function isCalledOnNullInput();

}
