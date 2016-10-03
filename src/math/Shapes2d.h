//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_SHAPES_H
#define TEST2_SHAPES_H


#include <glm/vec3.hpp>

template<typename T>
struct Rect {
    T x;
    T y;
    T width;
    T height;

    Rect():x(0),y(0),width(0),height(0) {}

    Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}

    Rect<T> intersect(const Rect& other) {
        T lx = std::max(x,other.x);
        T ly = std::max(y,other.y);
        T hx = std::min(x + width,other.x + other.width);
        T hy = std::min(y + height,other.y + other.height);
        return Rect<T>(lx,ly,hx,hy);
    }

    glm::tvec2<T,glm::precision::defaultp> xy() {
        return glm::tvec2<T,glm::precision::defaultp>(x,y);
    }

    T maxX() const {
        return x + width;
    }
    T maxY() const {
        return y + height;
    }
    T minX() const {
		return x;
	}
	T minY() const {
		return y;
	}
};




#endif //TEST2_SHAPES_H
