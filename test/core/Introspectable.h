//
// Created by Sam Bock on 9/9/16.
//

#ifndef INTROSPECTABLE_H
#define INTROSPECTABLE_H

#include <core/ArxString.h>

// generateMeta
struct Introspectable {
// beginFields
    int i;
    double d;
    Arx::String str;
// endFields

    Introspectable(int i, double d, const Arx::String &str) : i(i), d(d), str(str) {}
};

#include "Introspectable.h_generated.h" // Include Generated
#endif //TEST2_INTROSPECTABLE_H
