//
// Created by nvt on 8/3/16.
//
#include "UnitOfMeasure.h"
#include "catch.hpp"

TEST_CASE( "Units of measure conversions", "[measure]" ) {
    REQUIRE( Kilometers(3).in(Meters) == 3000.0f);
    REQUIRE( Meters(1000).in(Kilometers) == 1.0f);
}

TEST_CASE( "Units of measure equality", "[measure]" ) {
    REQUIRE( Meters(3) == Meters(3) );
    REQUIRE( Kilometers(1) == Meters(1000) );
    REQUIRE( Seconds(60) == Minutes(1));
}