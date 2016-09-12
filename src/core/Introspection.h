//
// Created by Sam Bock on 9/6/16.
//

#ifndef INTROSPECTION_H
#define INTROSPECTION_H

#include <typeinfo>
#include "ArxString.h"
#include "ArxFile.h"

using namespace Arx;

class Introspection {

};

// generateMeta
struct FieldInfo {
    const std::type_info& type;
    // beginFields
    String typeName;
    String name;
    int offset;
    // endFields

    String toString() const;


    FieldInfo(const type_info &type = typeid(FieldInfo)) : type(type) {}
};

struct StructInfo {
    Sequence<FieldInfo> fields;
    const std::type_info& type;
    String name;

    String toString() const;

    StructInfo(const type_info &type = typeid(StructInfo)) : type(type) {}
};

class ParsingContext;

class StructParser {
public:
    Sequence<StructInfo> parseStructData(ParsingContext& context, String raw);

    void amendHeaderFile(const File& file);
};

#endif //TEST2_INTROSPECTION_H
