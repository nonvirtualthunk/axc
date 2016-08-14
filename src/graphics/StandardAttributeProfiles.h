//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_STANDARDATTRIBUTEPROFILES_H
#define TEST2_STANDARDATTRIBUTEPROFILES_H


#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <core/Predef.h>
#include "VBO.h"

struct SimpleAttributeProfile {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::tvec4<uint8_t> color;

    inline void setColor(float r,float g,float b,float a) {
        color.r = (uint8_t) (r * 255);
        color.g = (uint8_t) (g * 255);
        color.b = (uint8_t) (b * 255);
        color.a = (uint8_t) (a * 255);
    }
    inline void setColor(glm::vec4 col) {
        setColor(col.r,col.g,col.b,col.a);
    }

    static const Arx::Sequence<Attribute> attributes;
};



#endif //TEST2_STANDARDATTRIBUTEPROFILES_H
