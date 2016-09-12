//
// Created by Sam Bock on 9/6/16.
//


#include <core/ArxString.h>
#include <catch.hpp>
#include <core/Introspection.h>
#include <Noto.h>
#include <core/Parsing.h>
#include <conf/AxmParser.h>
#include "Introspectable.h"

Arx::String raw = "\n"
		"#ifndef TMP_H\n"
		"#define TMP_H\n"
		"#include <somefile.h>\n"
        "\n"
		"// generateMeta \n"
        "struct Test {\n"
        "// beginFields\n"
        "\tint i;\n"
        "\tArx::String str;\n"
        "\tdouble d = 3.0;\n"
        "// endFields\n"
        "Test(int i) : i(i) {}\n"
        "};\n"
		"\n"
		"#endif\n";


TEST_CASE("StructParsingTest","[core][introspection]") {
    StructParser parser;

    ParsingContext ctxt;
    auto structData = parser.parseStructData(ctxt,raw);

    File tmp("/tmp/header.h");
    tmp.write(raw);

    parser.amendHeaderFile(tmp);
}

Arx::String introspectableConf = "{\n"
		"\ti : 4\n"
		"\td : 3.2\n"
		"\tstr : \"Hello\"\n"
		"}";
TEST_CASE("IntrospectableConfReading","[core][introspection]") {
    Introspectable introspectable(1,0.0,"test");

	AxmParser parser;
	ParsingContext ctxt;
	auto node = parser.parse(ctxt,introspectableConf);

	readFromConf(introspectable, *node);

	REQUIRE(introspectable.i == 4);
	REQUIRE(introspectable.d == 3.2);
	REQUIRE(introspectable.str == "Hello");
}

TEST_CASE("IntrospectableFieldListing","[core][introspection]") {
	const StructInfo& info = typeInfoForIntrospectable();
	REQUIRE(info.name == "Introspectable");
	REQUIRE(info.type == typeid(Introspectable));
	REQUIRE(info.type.hash_code() == typeid(Introspectable).hash_code());
	REQUIRE(info.fields[0].name == "i");
	REQUIRE(info.fields[0].type == typeid(int));
	REQUIRE(info.fields.size() == 3);
}