// START_GENERATED_CODE
#include <conf/AxmParser.h>
#include <core/Introspection.h>
#include "Introspectable.h"
void readFromConf(Introspectable& introspectable,const AxmNode& node) {
	(node % "i") >> introspectable.i;
	(node % "d") >> introspectable.d;
	(node % "str") >> introspectable.str;

}
StructInfo generatedStructInfoForIntrospectable() {
	StructInfo ret(typeid(Introspectable));
	ret.name = "Introspectable";
	FieldInfo i_field(typeid(int));
	i_field.name = "i";
	i_field.typeName = "int";
	ret.fields.add(i_field);
	FieldInfo d_field(typeid(double));
	d_field.name = "d";
	d_field.typeName = "double";
	ret.fields.add(d_field);
	FieldInfo str_field(typeid(Arx::String));
	str_field.name = "str";
	str_field.typeName = "Arx::String";
	ret.fields.add(str_field);
	return ret;
}
const StructInfo& typeInfoForIntrospectable() {
	static StructInfo ret = generatedStructInfoForIntrospectable();
	return ret;

};
