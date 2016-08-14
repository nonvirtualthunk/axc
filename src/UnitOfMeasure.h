//
// Created by nvt on 8/2/16.
//

#ifndef TEST1_UNITOFMEASURE_H
#define TEST1_UNITOFMEASURE_H

template<typename T> class UnitOfMeasure {
public:
    const float conversion;

    UnitOfMeasure(const float conv) : conversion(conv){

    }

    T operator()(float amount) const {
        return T(amount * conversion);
    }
};


template<typename T> class Measure {
protected:
    float inBaseUnits;

public:
    Measure(float inBase): inBaseUnits(inBase){}

    float in (UnitOfMeasure<T> unit) const {
        return inBaseUnits / unit.conversion;
    }

    bool operator==(const Measure<T> &rhs) const {
        return inBaseUnits == rhs.inBaseUnits;
    }

    bool operator!=(const Measure<T> &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Measure &rhs) const {
        return inBaseUnits < rhs.inBaseUnits;
    }

    bool operator>(const Measure &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Measure &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Measure &rhs) const {
        return !(*this < rhs);
    }

    T operator+(const Measure& rhs) const {
        return T(inBaseUnits + rhs.inBaseUnits);
    }
    T operator-(const Measure& rhs) const {
        return T(inBaseUnits - rhs.inBaseUnits);
    }
};

class Distance : public Measure<Distance> {
public:
    Distance(float inBase): Measure(inBase) {}
};
class Time : public Measure<Time> {
public:
    Time(float inBase);
};

using UnitOfDistance = UnitOfMeasure<Distance>;
using UnitOfTime = UnitOfMeasure<Time>;

const UnitOfDistance Meters(1.0f);
const UnitOfDistance Centimeters(0.01f);
const UnitOfDistance Millimeters(0.001f);
const UnitOfDistance Kilometers(1000.0f);

const UnitOfTime Seconds(1.0f);
const UnitOfTime Milliseconds(0.001f);
const UnitOfTime Minutes(60.0f);
const UnitOfTime Hours(Minutes.conversion * 60.0f);
const UnitOfTime Days(Hours.conversion * 24.0f);
const UnitOfTime Weeks(Days.conversion * 7.0f);
const UnitOfTime Years(Days.conversion * 365.25f);

//
//Time operator "" _seconds(long double amount) {
//    return Seconds(amount);
//}
//Distance operator "" _meters(long double amount) {
//    return Meters(amount);
//}

#endif //TEST1_UNITOFMEASURE_H
