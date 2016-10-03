//
// Created by nvt on 9/18/16.
//

#ifndef TEST2_PHYSICAL_H
#define TEST2_PHYSICAL_H

#include <core/AuxData.h>
#include <core/VoxelCoord.h>
#include <math/Shapes3d.h>
#include <math/MathHelpers.h>

// generateMeta
// struct Physical_t {
DECL_AUX_DATA(EntityAuxData, Physical) {
public:
    // beginFields
    Shape3<int>* shape = new Cube<int>(v3i(0,0,0),v3i(1,1,1));
    // endFields

	virtual ~Physical_t();
};

#include "Physical.h_generated.h" // Include Generated
#endif //TEST2_PHYSICAL_H
