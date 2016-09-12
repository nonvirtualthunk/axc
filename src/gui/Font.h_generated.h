// START_GENERATED_CODE
#ifndef AXM_PARSER_H
	#ifndef AXM_FWD_DECL
		class AxmNode;
		#define AXM_FWD_DECL
	#endif
#endif
#ifndef INTROSPECTION_H
	#ifndef INTROSPECTION_FWD_DECL
		class StructInfo;
		#define INTROSPECTION_FWD_DECL
	#endif
#endif
void readFromConf(FontConfig& fontconfig,const AxmNode& node);
const StructInfo& typeInfoForFontConfig();
