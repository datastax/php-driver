<?php

namespace Cassandra;

class CollectionsIntegrationTest extends BasicIntegrationTest {
    public function setUp() {
        parent::setUp();

        foreach ($this->compositeCassandraTypes() as $cassandraType) {
            if ($cassandraType[0] instanceof Type\UserType) {
                $this->createUserType($cassandraType[0]);
            }
        }
    }

    public function scalarCassandraTypes() {
        return array(
            array(Type::ascii(), array("a", "b", "c")),
            array(Type::bigint(), array(new Bigint("1"), new Bigint("2"), new Bigint("3"))),
            array(Type::blob(), array(new Blob("x"), new Blob("y"), new Blob("z"))),
            array(Type::boolean(), array(true, false, true, false)),
            #array(Type::decimal(), array(new Decimal(1.1), new Decimal(2.2), new Decimal(3.3))),
            array(Type::double(), array(1.1, 2.2, 3.3, 4.4)),
            array(Type::float(), array(new Float(1.0), new Float(2.2), new Float(2.2))),
            array(Type::inet(), array(new Inet("127.0.0.1"), new Inet("127.0.0.2"), new Inet("127.0.0.3"))),
            array(Type::text(), array("a", "b", "c", "x", "y", "z")),
            array(Type::timestamp(), array(new Timestamp(123), new Timestamp(456), new Timestamp(789))),
            array(Type::timeuuid(), array(new Timeuuid(0), new Timeuuid(1), new Timeuuid(2))),
            array(Type::uuid(),  array(new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2"),
                                       new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c3"),
                                       new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c4"))),
            array(Type::varchar(), array("a", "b", "c", "x", "y", "z")),
            array(Type::varint(), array(new Varint(1), new Varint(2), new Varint(3))),
        );
    }

    public function compositeCassandraTypes() {
        $collectionType = Type::collection(Type::varchar());
        $setType = Type::set(Type::varchar());
        $mapType = Type::map(Type::varchar(), Type::int());
        $tupleType = Type::tuple(Type::varchar(), Type::int(), Type::bigint());
        $userType = Type::userType("a", Type::varchar(), "b", Type::int(), "c", Type::bigint());
        $userType = $userType->withName(self::userTypeString($userType));

        return array(
            array($collectionType, array($collectionType->create("a", "b", "c"),
                                         $collectionType->create("d", "e", "f"),
                                         $collectionType->create("x", "y", "z"))),
            array($setType, array($setType->create("a", "b", "c"),
                                  $setType->create("d", "e", "f"),
                                  $setType->create("x", "y", "z"))),
            array($mapType, array($mapType->create("a", 1, "b", 2, "c", 3),
                                  $mapType->create("d", 4, "e", 5, "f", 6),
                                  $mapType->create("x", 7, "y", 8, "z", 9))),
            array($tupleType, array($tupleType->create("a", 1, new Bigint(2)),
                                    $tupleType->create("b", 3, new Bigint(4)),
                                    $tupleType->create("c", 5, new Bigint(6)))),
            array($userType, array($userType->create("a", "x", "b", 1, "c", new Bigint(2)),
                                   $userType->create("a", "y", "b", 3, "c", new Bigint(4)),
                                   $userType->create("a", "z", "b", 5, "c", new Bigint(6))))
        );
    }

    public function createUserType($userType) {
        $query  = "CREATE TYPE IF NOT EXISTS %s (%s)";
        $fieldsString = implode(", ", array_map(function ($name, $type) {
            return "$name $type";
        }, array_keys($userType->types()), $userType->types()));
        $query = sprintf($query, $this->userTypeString($userType), $fieldsString);
        $this->session->execute(new SimpleStatement($query));
    }

    public function createTable($type) {
        $query = "CREATE TABLE IF NOT EXISTS %s (key text PRIMARY KEY, value %s)";

        $cqlType = $this->typeString($type);
        $tableName = "table_" . str_replace(array("-"), "", (string)(new Uuid()));

        $query = sprintf($query, $tableName, $cqlType);

        $this->session->execute(new SimpleStatement($query));

        return $tableName;
    }

    public function createTableInsertAndVerifyValueByIndex($type, $value) {
        $key = "key";
        $options = new ExecutionOptions(array('arguments' => array($key, $value)));
        $this->createTableInsertAndVerifyValue($type, $key, $value, $options);
    }

    public function createTableInsertAndVerifyValueByName($type, $value) {
        $key = "key";
        $options = new ExecutionOptions(array('arguments' => array("key" => $key, "value" => $value)));
        $this->createTableInsertAndVerifyValue($type, $key, $value, $options);
    }

    protected function createTableInsertAndVerifyValue($type, $key, $value, $options) {
        $tableName = $this->createTable($type);

        $insertQuery = "INSERT INTO $tableName (key, value) VALUES (?, ?)";

        $this->session->execute(new SimpleStatement($insertQuery), $options);

        $selectQuery = "SELECT * FROM $tableName WHERE key = ?";

        $options = new ExecutionOptions(array('arguments' => array($key)));

        $result = $this->session->execute(new SimpleStatement($selectQuery), $options);

        $this->assertEquals(count($result), 1);

        $row = $result->first();

        $this->assertEquals($row['value'], $value);
        $this->assertTrue($row['value'] == $value);
        $this->assertEquals(count($row['value']), count($value));
    }

    public static function typeString($type) {
        if ($type instanceof Type\Tuple || $type instanceof Type\Collection ||
            $type instanceof Type\Map || $type instanceof Type\Set) {
            return sprintf("frozen<%s>", $type);
        } else {
            return (string)$type;
        }
    }

    public static function userTypeString($userType) {
        return sprintf("%s", implode("_", array_map(function ($name, $type) {
            return $name . str_replace(array("frozen", "<", " ", ",", ">"), "", $type);
        }, array_keys($userType->types()), $userType->types())));
    }
}
