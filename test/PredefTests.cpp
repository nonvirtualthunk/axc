//
// Created by nvt on 8/4/16.
//

#include <catch.hpp>
#include <core/Predef.h>

namespace predefTest {
    class A {

    };

    class B {

    };

    TEST_CASE("unique class id tests","[predef]") {
        int aId = getTypeId<A>();
        int bId = getTypeId<B>();

        REQUIRE(aId != bId);

        int aId2 = getTypeId<A>();

        REQUIRE(aId2 == aId);
    }
}