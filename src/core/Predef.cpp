//
// Created by nvt on 8/4/16.
//

#include <core/Predef.h>
#include <chrono>

long long int epochMillisSteady() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
}

long long int epochMillisSystem() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

atomic_int &IdCounter::getIdCounter() {
    static atomic_int idCounter(1);
    return idCounter;
}