//
// Created by nvt on 8/3/16.
//

#include <catch.hpp>
#include <Optional.h>
#include <core/Predef.h>

TEST_CASE("Basic optional usage","[containers]") {
    auto opt1 = some(6);

    REQUIRE(opt1.isPresent());
    bool check = false;
    opt1.ifPresent([&](int v){check = true;});
    REQUIRE(check);

    REQUIRE(opt1.orElse(7) == 6);

    auto opt2 = none<int>();

    REQUIRE(opt2.isEmpty());
    REQUIRE(!opt2.isPresent());

    opt2.ifPresent([&](int v){check = false;});

    REQUIRE(check);
    REQUIRE(opt2.orElse(7) == 7);
}

TEST_CASE("Optional equality","[containers]") {
    auto none1 = none<int>();
    auto none2 = none<int>();

    REQUIRE(none1 == none2);

    auto some1 = some(6);

    REQUIRE(none1 != some1);

    auto some2 = some(7);

    REQUIRE(some1 != some2);

    auto some3 = some(6);

    REQUIRE(some3 == some1);
}

TEST_CASE("Optional combinators","[containers]") {
    auto none1 = none<int>();

    REQUIRE(none1.map<int>([](int i){ return i * 2; }) == none1);
    REQUIRE(none1.map<float>([](int i){ return (float)i; }) == none<float>());

    auto some1 = some(4);

    REQUIRE(some1.map<float>([](int i){ return (float)i; }) == some(4.0f));
}

TEST_CASE("Optional matching","[containers]") {
    auto some1 = some(4);

    int value = -100;

    some1.ifPresent([&](int v) { value = v; });

    REQUIRE(value == 4);

    some1.ifEmpty([&](){ value = 0; });

    REQUIRE(value == 4);

    auto none1 = none<int>();

    none1.ifPresent([&](int v) { value = v; })
            .orIfEmpty([&](){ value = 0; });
    REQUIRE(value == 0);
}