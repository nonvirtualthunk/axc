//
// Created by nvt on 8/4/16.
//


#include <catch.hpp>
#include <ArxEnum.h>
#include <core/Predef.h>

class FooEnum : public ArxEnum {
public:
    FooEnum(const std::string &name) : ArxEnum(name, getTypeId<FooEnum>()) {}
};

class BarEnum : public ArxEnum {
public:
    BarEnum(const std::string &name) : ArxEnum(name, getTypeId<BarEnum>()) {}
};

FooEnum foo1("foo1");
FooEnum foo2("foo2");

BarEnum bar1("bar1");
BarEnum bar2("bar2");
BarEnum bar3("bar3");

TEST_CASE("ArxEnum basic functionality","[enum][core]") {
    REQUIRE(foo1.ordinal == 0);
    REQUIRE(foo1.ordinal != foo2.ordinal);

    REQUIRE(bar1.ordinal == 0);
    REQUIRE(bar2.ordinal != bar1.ordinal);
    REQUIRE(bar3.ordinal != bar1.ordinal);
    REQUIRE(bar3.ordinal != bar2.ordinal);

    REQUIRE(foo1.typeId != bar1.typeId);
    REQUIRE(foo1.typeId == foo2.typeId);
}

TEST_CASE("ArxEnum hash definition","[enum][core]") {
auto enumMap = Map<FooEnum,int>();

    enumMap.put(foo1, 6);
    enumMap.put(foo2, 3);

    REQUIRE(enumMap.get(foo1) == some(6));
}