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
class UserTypeIntegrationTest extends \PHPUnit_Framework_TestCase {
    /**
     * CQL for phone number user type
     */
    const PHONE_USER_TYPE_CQL = "CREATE TYPE phone (alias text, number text)";
    const PHONE_HOME_NUMBER = "555-911-1212";
    const PHONE_WORK_NUMBER = "650-389-6000";
    /**
     * CQL for address user type (requires phone user type being defined first)
     */
    const ADDRESS_USER_TYPE_CQL = "CREATE TYPE address (street text, zip int, phone_numbers set<frozen<phone>>)";
    const ADDRESS_STREET = "3975 Freedom Circle";
    const ADDRESS_ZIP = 95054;

    /**
     * Get the phone user type for assigning values.
     *
     * @return UserTypeValue Phone user type
     */
    public static function getPhoneUserType() {
        return new UserTypeValue(array(
            "alias" => \Cassandra::TYPE_TEXT,
            "number" => \Cassandra::TYPE_TEXT));
    }

    /**
     * Get the address user type for assigning values.
     *
     * @return UserTypeValue Address user type
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
     * @return UserTypeValue Valid address user type for testing
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
        $address->set("zip", self::ADDRESS_ZIP);
        $address->set("phone_numbers", $phoneNumbers);

        // Return the generated address
        return $address;
    }

    /**
     * Make assertions on a address user type.
     *
     * @param UserTypeValue $address Address user type to validate
     */
    public static function assertAddressValue(UserTypeValue $address) {
        // Verify the address
        self::assertEquals(self::getAddressUserType()->type(), $address->type());
        self::assertCount(3, $address);
        self::assertEquals(self::ADDRESS_STREET, $address->get("street"));
        self::assertEquals(self::ADDRESS_ZIP, $address->get("zip"));
        $phoneNumbers = $address->get("phone_numbers");
        self::assertInstanceOf('Cassandra\Set', $phoneNumbers);
        self::assertCount(2, $phoneNumbers);

        // Verify the home phone number
        $homePhone = $phoneNumbers->values()[0];
        self::assertCount(2, $homePhone);
        self::assertInstanceOf('Cassandra\UserTypeValue', $homePhone);
        self::assertEquals("Home", $homePhone->get("alias"));
        self::assertEquals(self::PHONE_HOME_NUMBER, $homePhone->get("number"));

        // Veryify the work phone number
        $workPhone = $phoneNumbers->values()[1];
        self::assertCount(2, $workPhone);
        self::assertInstanceOf('Cassandra\UserTypeValue', $workPhone);
        self::assertEquals("Work", $workPhone->get("alias"));
        self::assertEquals(self::PHONE_WORK_NUMBER, $workPhone->get("number"));
    }

    /**
     * @test
     */
    public function stub() {}
}