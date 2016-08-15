//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_SHAPES_H
#define TEST2_SHAPES_H


template<typename T>
struct Rect {
    T x;
    T y;
    T width;
    T height;

    Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}

    Rect<T> intersect(const Rect& other) {
        T lx = std::max(x,other.x);
        T ly = std::max(y,other.y);
        T hx = std::min(x + width,other.x + other.width);
        T hy = std::min(y + height,other.y + other.height);
        return Rect<T>(lx,ly,hx,hy);
    }
};

#endif //TEST2_SHAPES_H
