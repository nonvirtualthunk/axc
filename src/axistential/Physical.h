//
// Created by nvt on 9/18/16.
//

#ifndef TEST2_PHYSICAL_H
#define TEST2_PHYSICAL_H

#include <core/AuxData.h>
#include <core/VoxelCoord.h>

// generateMeta
DECL_AUX_DATA(EntityAuxData, Physical) {
    // beginFields
    VoxelCoord position;
    glm::ivec3 dimensions;
    // endFields
};


#endif //TEST2_PHYSICAL_H
