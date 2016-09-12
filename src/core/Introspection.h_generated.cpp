// START_GENERATED_CODE
#include <conf/AxmParser.h>
#include <core/Introspection.h>
void readFromConf(FieldInfo& fieldinfo,const AxmNode& node) {
	node % "typeName" >> fieldinfo.typeName;
	node % "name" >> fieldinfo.name;
	node % "offset" >> fieldinfo.offset;

}
