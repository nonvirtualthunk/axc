//
// Created by nvt on 9/18/16.
//

#include <math/ShapeHelpers.h>
#include "Physical.h"

#include "core/Predef.h"
#include "Physical.h_generated.cpp" // Include Generated

Physical_t::~Physical_t() {
	if (shape != nullptr) {
		delete shape;
	}
}
