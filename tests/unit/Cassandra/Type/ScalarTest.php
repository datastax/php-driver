<?php

namespace Cassandra\Type;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class ScalarTest extends \PHPUnit_Framework_TestCase
{
    public function testAllowCreatingTypes()
    {
        $this->assertEquals("some string", Type::varchar()->create("some string"));
    }

    /**
     * @dataProvider scalarTypes
     */
    public function testCompareEquals($type) {
        $this->assertTrue($type() == $type());
    }

    public function testCompareNotEquals() {
        $this->assertTrue(Type::ascii() != Type::bigint());
    }

    public function scalarTypes()
    {
        return array(
            array(function () { return Type::ascii();     }),
            array(function () { return Type::bigint();    }),
            array(function () { return Type::blob();      }),
            array(function () { return Type::boolean();   }),
            array(function () { return Type::counter();   }),
            array(function () { return Type::decimal();   }),
            array(function () { return Type::double();    }),
            array(function () { return Type::float();     }),
            array(function () { return Type::inet();      }),
            array(function () { return Type::int();       }),
            array(function () { return Type::text();      }),
            array(function () { return Type::timestamp(); }),
            array(function () { return Type::timeuuid();  }),
            array(function () { return Type::uuid();      }),
            array(function () { return Type::varchar();   }),
            array(function () { return Type::varint();    })
        );
    }
}
