//
// Created by Sam Bock on 9/20/16.
//

#ifndef TEST2_SHAPEHELPERS_H
#define TEST2_SHAPEHELPERS_H

#include <conf/AxmParser.h>
#include "Shapes3d.h"

template<typename T>
void operator>>(const AxmNode& node, glm::tvec3<T>& v) {
	if (!node.isEmpty()) {
		if (node.type == AxmNodeType::Array) {
			if (node.size() < 3) {
				Noto::error("Attempting to fill 3d coordinate from config array with too few values");
			} else {
				node[0] >> v.x;
				node[1] >> v.y;
				node[2] >> v.z;
			}
		} else if (node.type == AxmNodeType::Object) {
			node % "x" >> v.x;
			node % "y" >> v.y;
			node % "z" >> v.z;
		} else {
			Noto::error("Attempting to fill 3d coordinate from non-array, non-object: {}", node.type);
		}
	}
}

template<typename T>
void operator>>(const AxmNode &node, Shape3<T>* &v) {
	auto typeName = node.child("type").asStr();
	if (typeName == "cube") {
		Cube<T>* cube = new Cube<T>();
		node % "dimensions" >> cube->dimensions;
		node % "minCorner" >> cube->minCorner;
	}
}

#endif //TEST2_SHAPEHELPERS_H
