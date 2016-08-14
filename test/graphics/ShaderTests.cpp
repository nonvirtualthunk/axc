//
// Created by nvt on 8/13/16.
//

#include <catch.hpp>
#include <graphics/Shader.h>
#include <fstream>
#include <core/ArxString.h>
#include <core/Predef.h>



TEST_CASE("ShaderLoad","[shader][graphics]") {
    Shader shader;

    shader.load(readAll("resources/shaders/simple/SimpleShader.vertex").raw(),
                readAll("resources/shaders/simple/SimpleShader.fragment").raw());

    REQUIRE(shader.vertexShader.attribIndices.get("Vertex") == some(0));
    REQUIRE(shader.vertexShader.attribIndices.get("TexCoord") == some(1));
    REQUIRE(shader.vertexShader.attribIndices.get("Color") == some(2));
}