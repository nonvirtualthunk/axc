//
// Created by Sam Bock on 8/22/16.
//

#include <catch.hpp>
#include <core/Moddable.h>

TEST_CASE("ModdableCreation","[core]") {
    Moddable<float> vptr = M(3.0f);

    Moddable<float> fptr = M([](){ return 2.0f; });

    Moddable<float> cptr = M([&](){ return fptr.resolve() - 1.0f; });

    REQUIRE(vptr.resolve() == 3.0f);
    REQUIRE(fptr.resolve() == 2.0f);
    REQUIRE(cptr.resolve() == 1.0f);
};

TEST_CASE("ModdableAssignment","[core]") {
    auto tmp = M([]() { return 3.0f; });
    REQUIRE(tmp.resolve() == 3.0f);

    tmp = 2.0f;
    REQUIRE(tmp.resolve() == 2.0f);

    tmp = []() { return 1.2f; };
    REQUIRE(tmp.resolve() == 1.2f);
}