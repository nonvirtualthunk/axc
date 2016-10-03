//
// Created by nvt on 8/3/16.
//

#include <core/Predef.h>
#include <catch.hpp>
#include "Combinators.h"
using namespace comb;

TEST_CASE("Map function on vec", "[combinators]") {
    auto vec1 = Seq({1, 4, 2, 1});

    REQUIRE(vec1[0] == 1);
    REQUIRE(vec1[1] == 4);

    auto vec2 = fmap(vec1,[](int i) {return i * 2;});

    REQUIRE(vec2[0] == 2);
    REQUIRE(vec2[1] == 8);

    REQUIRE(vec1[0] == 1);
    REQUIRE(vec1[1] == 4);
}

TEST_CASE("Conversion to set","[containers]") {
    auto vec1 = Seq({2, 8, 4, 2});
    auto set1 = toStdSet(vec1);

    REQUIRE(set1.size() == 3);
    REQUIRE(contains(set1, 8));
}

TEST_CASE("Filter combinator","[combinators]") {
    auto vec1 = Seq({1,2,3,4});

    REQUIRE(filter(vec1,[](int v) { return v % 2 == 0; }) == Seq({2,4}));

    auto set1 = toStdSet(vec1);
    REQUIRE(set1.size() == 4);

    REQUIRE(filter(set1,[](int v) { return v % 2 == 0; }) == StdSet({2,4}));
    REQUIRE(filterNot(set1,[](int v) { return v % 2 == 0; }) == StdSet({1,3}));
}

TEST_CASE("Chained seq combinators","[combinators][seq]") {
    auto seq1 = Seq({1,2,3,4});
    auto res = seq1.filter(fni(i % 2 == 0)).map<int>(fni(i * 2));
    REQUIRE(res == Seq({4,8}));
}