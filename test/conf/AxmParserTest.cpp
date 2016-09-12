//
// Created by Sam Bock on 9/4/16.
//


#include <catch.hpp>
#include <conf/AxmParser.h>
#include <core/Parsing.h>


Arx::String testStr1("{\n"
        "numberValue : 3.5\n"
        "boolValue: true\n"
        "strValue: \"test\"\n"
        "objectValue: {\n"
        "\tnested: 2\n"
        "\tnestedArray: [1,2,3]\n"
        "}\n"
        "arrayValue: [true,false]\n"
        "}");

TEST_CASE("AxmParserBasicUsage","[conf][core]") {

    AxmParser parser;

    ParsingContext ctxt;
    auto nodePtr = parser.parse(ctxt, testStr1);
    auto& node = *nodePtr;

    REQUIRE(node.child("numberValue").asDouble() == 3.5);
    REQUIRE(node("boolValue").asBool());
    REQUIRE(node("strValue").asStr() == "test");
    REQUIRE(node("objectValue")("nested").asInt() == 2);
    REQUIRE(node["objectValue"]["nestedArray"][0].asInt() == 1);
    REQUIRE(node["arrayValue"][1].asBool() == false);
}