//
// Created by nvt on 8/5/16.
//


#include <core/Predef.h>
#include <catch.hpp>
#include <core/AuxData.h>

template<typename Assoc>
class FooAuxData : public AuxDataType<FooAuxData<void>,Assoc> {};

template<typename Assoc>
class BarAuxData : public AuxDataType<BarAuxData<void>,Assoc> {};


DECL_AUX_DATA(FooAuxData,Foo1) {
public:
    int a;

    Foo1_t() : a(0) {}
};

DECL_AUX_DATA(FooAuxData,Foo2) {
public:
    std::string str = "Hello";
    Foo2_t() {}
private:
    Foo2_t(const Foo2_t&) = default;
};

DECL_AUX_DATA(BarAuxData,Bar1) {
public:
    double d = 3.9;
};

TEST_CASE("Aux type id assignment","[core][aux]") {
    REQUIRE(Foo1.id != Foo2.id);
    REQUIRE(Bar1.id == 0);
    REQUIRE((Foo1.id == 0 || Foo2.id == 0));
}

TEST_CASE("Aux data creation and id assignment","[core][aux]") {
    AuxDataContainer<FooAuxData<void>> adc;

    auto& foo1 = adc(Foo1);

    REQUIRE(foo1.a == 0);

    foo1.a = 3;

    auto& reRequested = adc(Foo1);

    REQUIRE(reRequested.a == foo1.a);

    auto& foo2 = adc(Foo2);

    REQUIRE(foo2.str == "Hello");

    AuxDataContainer<FooAuxData<void>> adc2;

    auto& foo1_2 = adc2(Foo1);

    REQUIRE(foo1_2.a != foo1.a);
}

TEST_CASE("HasAuxData usage","[core][aux]") {
    HasAuxData<BarAuxData<void>> had;

    auto& bar1 = had(Bar1);

    REQUIRE(bar1.d == 3.9);
}