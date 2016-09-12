//
// Created by Sam Bock on 9/6/16.
//

#include "Introspection.h"
#include "Predef.h"
#include "Metrics.h"


#include <core/Parsing.h>

using namespace Arx;

struct ParseState : public ParseStateBase {
    ParseState(String &raw, ParsingContext &context) : ParseStateBase(raw, context) {}


};

String nextStatement(ParseState& state) {
    String stmt = state.takeUntilWhitespace(0);
    stmt.trim();
    return stmt;
}
String nextLine(ParseState& state) {
    String line = state.takeUntil('\n',0);
    line.trim();
    return line;
}

bool advanceToStruct(ParseState& state) {
    bool considerNext = false;
    while (state.hasNext()) {
        String stmt = nextStatement(state);
        if (stmt == "generateMeta") {
            considerNext = true;
        } else if (stmt == "struct" && considerNext) {
            return true;
        } else if (stmt == "START_GENERATED_CODE") {
            return false;
        }
    }
    return false;
}

void parseFields(ParseState& state, StructInfo& info) {
    while (state.hasNext()) {
        String statement = state.takeUntil(';', 0);
        statement.trim();
        if (statement.contains("endFields")) {
            return;
        }
//        section.takeWhile(fnc(c != '='));
        Sequence<String> sections = statement.split(' ');
        if (sections.size() >= 2) {
            FieldInfo field;
            field.typeName = sections[0];
            field.name = sections[1];

            info.fields.add(field);
        } else {
            Noto::error("Malformed field in struct, full text\"{}\"", statement);
        }
    }
}

StructInfo parseStruct(ParseState& state) {
    StructInfo ret;

    ret.name = nextStatement(state);
    state.advanceUntil('{');
    while (state.hasNext()) {
        String line = nextLine(state);
        if (line.contains("beginFields")) {
            parseFields(state, ret);
            break;
        }
    }
    return ret;
}

Sequence<StructInfo> StructParser::parseStructData(ParsingContext &context, String raw) {
    Sequence<StructInfo> ret;

    ParseState state(raw, context);
    while (advanceToStruct(state)) {
        ret.add(parseStruct(state));
    }
    return ret;

}

String generateReadFunctionDecl(const StructInfo& info) {
    return Format::format("void readFromConf({}& {},const AxmNode& node)",info.name,info.name.toLower());
}

String generateReadFunction(const StructInfo& info) {
    String body;
    for (const FieldInfo& field : info.fields) {
        body.append(Format::format("\t(node % \"{}\") >> {}.{};\n", field.name, info.name.toLower(), field.name));
    }

    String decl = generateReadFunctionDecl(info);
    return Format::format("{} {\n{}\n}",decl,body);
}

String generateIntrospectionFunctionDecl(const StructInfo& info) {
	return Format::format("const StructInfo& typeInfoFor{}()",info.name);
}

String generateIntrospectionFunction(const StructInfo& info) {
	String helper = Format::format("StructInfo generatedStructInfoFor{}() {\n"
										   "\tStructInfo ret(typeid({}));\n", info.name, info.name);

	helper.append(Format::format("\tret.name = \"{}\";\n", info.name));
	for (const FieldInfo& field : info.fields) {
		Arx::String fieldName = Format::format("{}_field", field.name);
		helper.append(Format::format("\tFieldInfo {}(typeid({}));\n"
										   "\t{}.name = \"{}\";\n"
										   "\t{}.typeName = \"{}\";\n"
										   "\tret.fields.add({});\n",
									 fieldName, field.typeName, fieldName, field.name, fieldName, field.typeName, fieldName));
	}
	helper.append("\treturn ret;"
						  "\n}");

	String body = Format::format("\tstatic StructInfo ret = generatedStructInfoFor{}();\n"
										 "\treturn ret;\n", info.name);

	String decl = generateIntrospectionFunctionDecl(info);
	return Format::format("{}\n{} {\n{}\n}",helper, decl, body);
}

void StructParser::amendHeaderFile(const File& file) {
	MetricsTimer timer(fmt("Code generation for {}", file.fileName()));

    String raw = file.readAllText();

    ParsingContext context;
    Sequence<StructInfo> infos = parseStructData(context,raw);

    Arx::String fileName = file.fileName();
    Arx::File generatedHeaderFile = file.parent().relativeFile(fileName + "_generated.h");
    Arx::File generatedCppFile = file.parent().relativeFile(fileName + "_generated.cpp");


    String genHead;
    genHead.append("// START_GENERATED_CODE\n");
    genHead.append("#ifndef AXM_PARSER_H\n"
                                 "\t#ifndef AXM_FWD_DECL\n"
                                 "\t\tclass AxmNode;\n"
                                 "\t\t#define AXM_FWD_DECL\n"
                                 "\t#endif\n"
                                 "#endif\n"
						   "#ifndef INTROSPECTION_H\n"
						   "\t#ifndef INTROSPECTION_FWD_DECL\n"
						   "\t\tclass StructInfo;\n"
						   "\t\t#define INTROSPECTION_FWD_DECL\n"
						   "\t#endif\n"
						   "#endif\n");

    String genCpp;
    genCpp.append("// START_GENERATED_CODE\n"
                          "#include <conf/AxmParser.h>\n"
                          "#include <core/Introspection.h>\n");
    genCpp.append(Format::format("#include \"{}\"\n", fileName));

    for (const StructInfo& info : infos) {
        Noto::info("\tProcessing struct info: {}", info.name);
        genHead.append(generateReadFunctionDecl(info).raw());
        genHead.append(";\n");
		genHead.append(Format::format("{};\n",generateIntrospectionFunctionDecl(info).raw()));

        genCpp.append(generateReadFunction(info).raw());
        genCpp.append("\n");
		genCpp.append(Format::format("{};\n",generateIntrospectionFunction(info).raw()));
    }

	String existingGenHead = generatedHeaderFile.exists() ? generatedHeaderFile.readAllText() : "";
	String existingGenCpp = generatedCppFile.exists() ? generatedCppFile.readAllText() : "";
	if (genHead.size() != existingGenHead.size() || genHead != existingGenHead) {
		Noto::info("\tRewriting generated header for {}", file.fileName());
    	generatedHeaderFile.write(genHead);
	}
	if (genCpp.size() != existingGenCpp.size() || genCpp != existingGenCpp) {
		Noto::info("\tRewriting generated cpp for {}", file.fileName());
    	generatedCppFile.write(genCpp);
	}

    Arx::String includeGeneratedIdent = "// Include Generated";
    int existingIncludeEnd = raw.reverseFind(includeGeneratedIdent);
    if (existingIncludeEnd == -1) {
        Noto::info("\tRewriting file to include generated header {}", generatedHeaderFile.path);
        int lindex = raw.reverseFind("#endif");
        raw.insert(Format::format("#include \"{}\" {}\n", generatedHeaderFile.fileName(), includeGeneratedIdent), lindex);

        file.write(raw);

        Arx::File mainCpp = file.withExtension(".cpp");
        Arx::String cppRaw;
        if (mainCpp.exists()) {
            cppRaw = mainCpp.readAllText();
        }

        if (!cppRaw.contains(includeGeneratedIdent)) {
            Noto::info("Rewriting cpp file to include generated cpp {}", generatedCppFile.path);
            cppRaw.append(Format::format("#include \"{}\" {}\n", generatedCppFile.fileName(), includeGeneratedIdent));

            mainCpp.write(cppRaw);
        }
    }

	timer.printElapsed();
}

String FieldInfo::toString() const {
    return Format::format("Field({} {})", typeName, name);
}

String StructInfo::toString() const {
    String ret = "Struct(";
    ret.append(name);
    ret.append(") {\n");
    for (const FieldInfo& info : fields) {
        ret.append(info.toString());
        ret.append("\n");
    }
    ret.append("}");
    return ret;
}
