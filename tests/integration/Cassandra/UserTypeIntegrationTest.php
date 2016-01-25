<?php

namespace Cassandra;

    /**
     * Copyright 2015-2016 DataStax, Inc.
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

/**
 * User type integration tests.
 *
 * @cassandra-version-2.1
 */
class UserTypeIntegrationTest extends BasicIntegrationTest {
    /**
     * CQL for phone number user type
     */
    const PHONE_USER_TYPE_CQL = "CREATE TYPE phone (alias text, number text)";
    /**
     * Home phone number to use for inserts and assertions
     */
    const PHONE_HOME_NUMBER = "555-911-1212";
    /**
     * Work phone number to use for inserts and assertions
     */
    const PHONE_WORK_NUMBER = "650-389-6000";
    /**
     * CQL for address user type (requires phone user type being defined first)
     */
    const ADDRESS_USER_TYPE_CQL = "CREATE TYPE address (street text, zip int, phone_numbers set<frozen<phone>>)";
    /**
     * Street address to use for inserts and assertions
     */
    const ADDRESS_STREET = "3975 Freedom Circle";
    /**
     * Zip code to use for inserts and assertions
     */
    const ADDRESS_ZIP_CODE = 95054;

    /**
     * Setup the database for the user type tests.
     *
     * @before
     */
    public function setupTest() {
        // Process parent setup steps
        parent::setupTest();

        // Create the user types
        $this->session->execute(new SimpleStatement(self::PHONE_USER_TYPE_CQL));
        $this->session->execute(new SimpleStatement(self::ADDRESS_USER_TYPE_CQL));

        // Create the table
        $query = "CREATE TABLE " . $this->tableNamePrefix .
            " (key timeuuid PRIMARY KEY, value " .
            "frozen<address>)";
        $this->session->execute(new SimpleStatement($query));
    }

    /**
     * Get the phone user type for assigning values.
     *
     * @return \Cassandra\UserTypeValue Phone user type
     */
    public static function getPhoneUserType() {
        return new UserTypeValue(array(
            "alias" => \Cassandra::TYPE_TEXT,
            "number" => \Cassandra::TYPE_TEXT));
    }

    /**
     * Get the address user type for assigning values.
     *
     * @return \Cassandra\UserTypeValue Address user type
     */
    public static function getAddressUserType() {
        $phoneNumbers = new Set(self::getPhoneUserType()->type());
        return new UserTypeValue(array(
            "street" => \Cassandra::TYPE_TEXT,
            "zip" => \Cassandra::TYPE_INT,
            "phone_numbers" => $phoneNumbers->type()
        ));
    }

    /**
     * Generate a valid address user type with values that can be used for
     * testing.
     *
     * @return \Cassandra\UserTypeValue Valid address user type for testing
     */
    public static function generateAddressValue() {
        // Create the phone numbers and add them to a set
        $homePhone = UserTypeIntegrationTest::getPhoneUserType();
        $homePhone->set("alias", "Home");
        $homePhone->set("number", self::PHONE_HOME_NUMBER);
        $workPhone = UserTypeIntegrationTest::getPhoneUserType();
        $workPhone->set("alias", "Work");
        $workPhone->set("number", self::PHONE_WORK_NUMBER);
        $phoneNumbers = new Set($homePhone->type());
        $phoneNumbers->add($homePhone);
        $phoneNumbers->add($workPhone);

        // Create the address and add the set of phone numbers
        $address = UserTypeIntegrationTest::getAddressUserType();
        $address->set("street", self::ADDRESS_STREET);
        $address->set("zip", self::ADDRESS_ZIP_CODE);
        $address->set("phone_numbers", $phoneNumbers);

        // Return the generated address
        return $address;
    }

    /**
     * Make assertions on a address user type.
     *
     * @param \Cassandra\UserTypeValue $address Address user type to validate
     * @param \Cassandra\UserTypeValue $expected Expected address user type
     *                                           value
     *                                           (DEFAULT: self::generateAddressValue())
     */
    public static function assertAddressValue(UserTypeValue $address, UserTypeValue $expected = null) {
        // Determine if the expected value should be defaulted
        if (is_null($expected)) {
            $expected = self::generateAddressValue();
        }

        // Verify the address
        self::assertEquals($expected->type(), $address->type());
        self::assertCount(count($expected), $address);
        self::assertEquals($expected->get("street"), $address->get("street"));
        self::assertEquals($expected->get("zip"), $address->get("zip"));
        $expectedPhoneNumbers = $address->get("phone_numbers");
        $phoneNumbers = $address->get("phone_numbers");
        if (!is_null($phoneNumbers)) {
            self::assertInstanceOf('Cassandra\Set', $phoneNumbers);
            $expectedNumberOfPhoneNumbers = count($expectedPhoneNumbers);
            self::assertCount($expectedNumberOfPhoneNumbers, $phoneNumbers);

            // Verify phone numbers
            self::assertEquals($expectedPhoneNumbers, $phoneNumbers);
            if (count($expectedPhoneNumbers) > 0) {
                foreach (range(0, $expectedNumberOfPhoneNumbers - 1) as $i)
                    $expectedNumber = $expectedPhoneNumbers->values()[$i];
                $number = $phoneNumbers->values()[$i];
                self::assertCount(count($expectedNumber), $number);
                self::assertInstanceOf('Cassandra\UserTypeValue', $number);
                self::assertEquals($expectedNumber->get("alias"), $number->get("alias"));
                self::assertEquals($expectedNumber->get("number"), $number->get("number"));
            }
        }
    }

    /**
     * Insert an address into the table.
     *
     * @param \Cassandra\UserTypeValue $address User type to insert into the
     *                                          table
     * @return \Cassandra\Timeuuid Key used during insert
     */
    private function insertAddress($address) {
        // Assign the values for the statement
        $key = new Timeuuid();
        $values = array(
            $key,
            $address
        );

        // Insert the value into the table
        $query = "INSERT INTO " . $this->tableNamePrefix . " (key, value) VALUES (?, ?)";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => $values));
        $this->session->execute($statement, $options);

        // Return the key for asserting the user type
        return $key;
    }

    /**
     * Select the address from the table.
     *
     * @param \Cassandra\Timeuuid $key Key to use to select user type value
     * @return \Cassandra\UserTypeValue User type retrieved from the server
     */
    private function selectAddress($key) {
        // Select the user type
        $query = "SELECT value FROM " . $this->tableNamePrefix . " WHERE key=?";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => array($key)));
        $rows = $this->session->execute($statement, $options);

        // Ensure the user type is valid
        $this->assertCount(1, $rows);
        $row = $rows->first();
        $this->assertNotNull($row);
        $this->assertArrayHasKey("value", $row);
        $userType = $row["value"];
        $this->assertInstanceOf('Cassandra\UserTypeValue', $userType);

        // Return the user type
        return $userType;
    }

    /**
     * User type using a complete user type value.
     *
     * This test will ensure that the PHP driver supports the user types. This
     * test uses a complete user type will all values assigned for the
     * associated user type.
     *
     * @test
     * @ticket PHP-57
     */
    public function completeUserType() {
        $key = $this->insertAddress($this->generateAddressValue());
        $this->assertAddressValue($this->selectAddress($key));
    }

    /**
     * User type using a partial user type value.
     *
     * This test will ensure that the PHP driver supports the user types. This
     * test uses a partial user type where some values will not be assigned.
     *
     * @test
     * @ticket PHP-57
     */
    public function partialUserType() {
        // Alias missing from a single number
        $phone = $this->getPhoneUserType();
        $phone->set("number", "000-000-0000");
        $numbers = new Set($phone->type());
        $numbers->add($phone);
        $address = UserTypeIntegrationTest::getAddressUserType();
        $address->set("street", "123 Missing Alias Street");
        $address->set("zip", 00000);
        $address->set("phone_numbers", $numbers);
        $key = $this->insertAddress($address);
        $this->assertAddressValue($this->selectAddress($key), $address);

        // Missing/NULL values during insert
        $address = UserTypeIntegrationTest::getAddressUserType();
        $address->set("street", "1 Furzeground Way");
        $key = $this->insertAddress($address);
        $address->set("zip", null); // Add null 'zip' to assert properly (Server will default null values)
        $address->set("phone_numbers", null); // Add null 'phone_numbers' (same as above)
        $this->assertAddressValue($this->selectAddress($key), $address);
    }

    /**
     * Frozen decoration required for user type.
     *
     * This test will ensure that the PHP driver throws an exception when
     * interacting with Cassandra 2.1+ (< 3.0) and the frozen decoration is
     * omitted.
     *
     * @test
     * @ticket PHP-57
     * @expectedException \Cassandra\Exception\InvalidQueryException
     * @expectedExceptionMessage Non-frozen User-Defined types are not
     *                           supported, please use frozen<>
     * @cassandra-version-less-3
     */
    public function frozenRequired() {
        $statement = new SimpleStatement("CREATE TYPE frozen_required (id uuid, address address)");
        $this->session->execute($statement);
    }

    /**
     * Unavailable user type referenced.
     *
     * This test will ensure that the PHP driver throws an exception when
     * referencing a non-existent user type.
     *
     * @test
     * @ticket PHP-57
     * @expectedException \Cassandra\Exception\InvalidQueryException
     * @expectedExceptionMessageRegExp |Unknown type .*.user_type_unavailable|
     */
    public function unavailableUserType() {
        $statement = new SimpleStatement("CREATE TABLE unavailable (id uuid PRIMARY KEY, unavailable frozen<user_type_unavailable>)");
        $this->session->execute($statement);
    }

    /**
     * Invalid value assigned to user type .
     *
     * This test will ensure that the PHP driver throws and exception when
     * assigning a value to a user type that is not valid for that type.
     *
     * @test
     * @ticket PHP-57
     * @expectedException \Cassandra\Exception\InvalidQueryException
     */
    public function invalidAddressUserTypeAssignedValue() {
        $invalidValue = $this->getPhoneUserType();
        $invalidValue->set("alias", "Invalid Value");
        $invalidValue->set("number", "800-555-1212");
        $this->insertAddress($invalidValue);
    }

    /**
     * Invalid value assigned to user type .
     *
     * This test will ensure that the PHP driver throws and exception when
     * assigning a value to a user type that is not valid for that type.
     *
     * @test
     * @ticket PHP-57
     * @expectedException \Cassandra\Exception\InvalidQueryException
     */
    public function invalidPhoneUserTypeAssignedValue() {
        // Create a new table
        $this->session->execute(new SimpleStatement("CREATE TABLE invalidphone (key int PRIMARY KEY, value frozen<phone>)"));
        $invalidValue = $this->generateAddressValue();

        // Bind and insert the invalid phone value
        $values = array(
            1,
            $invalidValue
        );
        $query = "INSERT INTO invalidphone (key, value) VALUES (?, ?)";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => $values));
        $this->session->execute($statement, $options);
    }
}