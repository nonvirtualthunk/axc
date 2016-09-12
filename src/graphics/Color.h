//
// Created by Sam Bock on 8/15/16.
//

#ifndef TEST2_COLOR_H
#define TEST2_COLOR_H

#include <cstdint>
#include <glm/vec4.hpp>

class Color : public glm::tvec4<uint8_t> {
public:
    Color() {}

    Color(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : tvec4(a, b, c, d) {}

    static Color fromPercent(float a, float b, float c, float d) {
        return Color((uint8_t) (a * 255), (uint8_t) (b * 255), (uint8_t) (c * 255), (uint8_t) (d * 255));
    }

    Color operator*(const glm::vec4& v);

    inline void setF(float r,float g,float b,float a) {
        this->r = (uint8_t) (r * 255);
        this->g = (uint8_t) (g * 255);
        this->b = (uint8_t) (b * 255);
        this->a = (uint8_t) (a * 255);
    }
    inline void set(glm::vec4 col) {
        setF(col.r,col.g,col.b,col.a);
    }
};

#endif //TEST2_COLOR_H
