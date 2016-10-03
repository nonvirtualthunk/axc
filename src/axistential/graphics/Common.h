//
// Created by Sam Bock on 9/24/16.
//

#ifndef TEST2_COMMON_H
#define TEST2_COMMON_H

//	val cardinals : Array[ReadVec3i] = Array[ReadVec3i](Vec3i(-1,0,0),Vec3i(0,-1,0),Vec3i(0,0,-1),Vec3i(1,0,0),Vec3i(0,1,0),Vec3i(0,0,1))
//	val expandedCardinals : Array[ReadVec3i] = Array[ReadVec3i](	Vec3i(-1,0,0),Vec3i(0,-1,0),Vec3i(0,0,-1),Vec3i(1,0,0),Vec3i(0,1,0),Vec3i(0,0,1),
//																	Vec3i(-1,0,1),Vec3i(0,-1,1),Vec3i(1,0,1),Vec3i(0,1,1),
//																	Vec3i(-1,0,-1),Vec3i(0,-1,-1),Vec3i(1,0,-1),Vec3i(0,1,-1),
//																	Vec3i(-1,-1,0),Vec3i(-1,1,0),Vec3i(1,-1,0),Vec3i(1,1,0))
//	val fullCardinals = Array[ReadVec3i](	Vec3i(-1,0,0),Vec3i(0,-1,0),Vec3i(0,0,-1),Vec3i(1,0,0),Vec3i(0,1,0),Vec3i(0,0,1),
//											 Vec3i(-1,0,1),Vec3i(0,-1,1),Vec3i(1,0,1),Vec3i(0,1,1),
//											 Vec3i(-1,0,-1),Vec3i(0,-1,-1),Vec3i(1,0,-1),Vec3i(0,1,-1),
//											 Vec3i(-1,-1,-1),Vec3i(-1,1,-1),Vec3i(1,-1,-1),Vec3i(1,1,-1),
//											 Vec3i(-1,-1,1),Vec3i(-1,1,1),Vec3i(1,-1,1),Vec3i(1,1,1))

#include <glm/vec3.hpp>

extern const glm::vec3 cubePoints[6][4];
extern const glm::ivec3 cardinals[6];

#endif //TEST2_COMMON_H
