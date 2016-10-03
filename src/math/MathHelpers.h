//
// Created by Sam Bock on 9/26/16.
//

#ifndef TEST2_MATHHELPERS_H
#define TEST2_MATHHELPERS_H

#include <glm/vec3.hpp>

typedef glm::ivec3 Vec3i;
typedef glm::vec3 Vec3f;

inline glm::ivec3 v3i(int x, int y, int z) {
	return glm::ivec3(x,y,z);
}

inline glm::vec3 v3(float x,float y,float z) {
	return glm::vec3(x,y,z);
}

#endif //TEST2_MATHHELPERS_H
