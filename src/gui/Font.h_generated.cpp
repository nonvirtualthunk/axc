// START_GENERATED_CODE
#include <conf/AxmParser.h>
#include <core/Introspection.h>
#include "Font.h"
void readFromConf(FontConfig& fontconfig,const AxmNode& node) {
	(node % "rangeScale") >> fontconfig.rangeScale;
	(node % "pixelSize") >> fontconfig.pixelSize;
	(node % "useMSDF") >> fontconfig.useMSDF;
	(node % "seed") >> fontconfig.seed;
	(node % "angleThreshold") >> fontconfig.angleThreshold;

}
StructInfo generatedStructInfoForFontConfig() {
	StructInfo ret(typeid(FontConfig));
	ret.name = "FontConfig";
	FieldInfo rangeScale_field(typeid(float));
	rangeScale_field.name = "rangeScale";
	rangeScale_field.typeName = "float";
	ret.fields.add(rangeScale_field);
	FieldInfo pixelSize_field(typeid(int));
	pixelSize_field.name = "pixelSize";
	pixelSize_field.typeName = "int";
	ret.fields.add(pixelSize_field);
	FieldInfo useMSDF_field(typeid(bool));
	useMSDF_field.name = "useMSDF";
	useMSDF_field.typeName = "bool";
	ret.fields.add(useMSDF_field);
	FieldInfo seed_field(typeid(int));
	seed_field.name = "seed";
	seed_field.typeName = "int";
	ret.fields.add(seed_field);
	FieldInfo angleThreshold_field(typeid(float));
	angleThreshold_field.name = "angleThreshold";
	angleThreshold_field.typeName = "float";
	ret.fields.add(angleThreshold_field);
	return ret;
}
const StructInfo& typeInfoForFontConfig() {
	static StructInfo ret = generatedStructInfoForFontConfig();
	return ret;

};
