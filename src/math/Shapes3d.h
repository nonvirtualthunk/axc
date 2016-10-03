//
// Created by Sam Bock on 9/21/16.
//

#ifndef TEST2_SHAPES3D_H
#define TEST2_SHAPES3D_H

#include <glm/vec3.hpp>

template<typename T>
class Shape3 {
public:
    virtual glm::tvec3<T> center() const = 0;
	virtual glm::tvec3<T> min() const = 0;
	virtual glm::tvec3<T> max() const = 0;
	virtual void translate(glm::tvec3<T> delta) = 0;

	virtual ~Shape3() {}

	template<typename std::enable_if<std::is_same<T, int>::value>::type* = nullptr>
	void foreach(std::function<void(int,int,int)> func) {
		auto start = min();
		auto end = max();
		for (int z = start.z; z <= end.z; ++z) {
			for (int y = start.y; y <= end.y; ++y) {
				for (int x = start.x; x <= end.x; ++x) {
					func(x,y,z);
				}
			}
		}
	};
};

template<typename T>
struct Cube : public Shape3<T> {

	Cube() {}

	Cube(const glm::ivec3 &minCorner, const glm::ivec3 &max) : minCorner(minCorner) {
		dimensions = glm::ivec3(max.x-minCorner.x+1,max.y-minCorner.y+1,max.z-minCorner.z+1);
	}

	glm::tvec3<T> minCorner;
    glm::tvec3<T> dimensions = glm::tvec3<T>(1,1,1);

	virtual glm::tvec3<T> center() const override {
		return minCorner + dimensions/2;
	}

	virtual glm::tvec3<T> min() const override {
		return minCorner;
	}

	virtual glm::tvec3<T> max() const override {
		return glm::tvec3<T>(minCorner.x + dimensions.x - 1, minCorner.y + dimensions.y - 1,minCorner.z + dimensions.z - 1);
	}

	virtual void translate(glm::tvec3<T> delta) override {
		minCorner += delta;
	}
};

#endif //TEST2_SHAPES3D_H
