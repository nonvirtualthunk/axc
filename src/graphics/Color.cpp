//
// Created by nvt on 8/21/16.
//
#include "Color.h"

Color Color::operator*(const glm::vec4& v) {
    return Color((uint8_t) (r * v.r), (uint8_t) (g * v.g), (uint8_t) (b * v.b), (uint8_t) (a * v.a));
}