//
// Created by nvt on 8/2/16.
//

#include "UnitOfMeasure.h"


//UnitOfDistance::UnitOfDistance(const float conv) : UnitOfMeasure(conv) {}

//#include<gtest/gtest.h>
//
//TEST(UnitOfMeasureTest, CreationAndConversion) {
//    //ASSERT_EQ(Kilometers(2).in(Meters),2000.0f) << "Kilometer to meter conversion failed";
//    ASSERT_EQ(3,2) << "TEST";
//}
Time::Time(float inBase) : Measure(inBase) {}