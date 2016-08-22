//
// Created by nvt on 8/13/16.
//

#include <catch.hpp>
#include <graphics/Shader.h>
#include <fstream>
#include <core/ArxString.h>
#include <core/Predef.h>
#include <application/ResourceManager.h>


TEST_CASE("ShaderLoad","[shader][graphics]") {
    auto shader = ResourceManager::shader("shaders/simple/SimpleShader");

    REQUIRE(shader->vertexShader.attribIndices.get("Vertex") == some(0));
    REQUIRE(shader->vertexShader.attribIndices.get("TexCoord") == some(1));
    REQUIRE(shader->vertexShader.attribIndices.get("Color") == some(2));
}