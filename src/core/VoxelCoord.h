//
// Created by Sam Bock on 8/9/16.
//

#ifndef TEST2_VOXELCOORD_H
#define TEST2_VOXELCOORD_H

#include <glm/vec3.hpp>
#include <functional>

class VoxelCoord : public glm::ivec3 {

public:
    VoxelCoord(const int &a, const int &b, const int &c) : tvec3(a, b, c) {}
    VoxelCoord(const glm::ivec3 &v) : tvec3(v) {}
    VoxelCoord(const VoxelCoord& v) : tvec3(v.x,v.y,v.z) {}

    size_t hash() const {
        return (size_t) ((x * 73856093L) xor (y * 19349663L) xor (z * 83492791L));
    }
};

#endif //TEST2_VOXELCOORD_H
