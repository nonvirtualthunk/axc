//
// Created by Sam Bock on 8/12/16.
//


#include <catch.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <graphics/VBO.h>
#include <containers/Seq.h>
#include <core/Predef.h>




struct SimplePoint {
    glm::vec3 position;
    glm::vec4 color;

    static const Arx::Sequence<Attribute> attributes;
};
const Arx::Sequence<Attribute> SimplePoint::attributes = Seq({ Attribute(3,GL_FLOAT), Attribute(4,GL_FLOAT) });


TEST_CASE("VBODataTest","[graphics]") {
    VBO<SimplePoint,uint16_t> vbo;

    SimplePoint* points = vbo.addQuad();
    for (int i = 0; i < 4; ++i) {
        points[i].position = glm::vec3(i,i+1,i+2);
        points[i].color = glm::vec4(1,1,1,1);
    }
}