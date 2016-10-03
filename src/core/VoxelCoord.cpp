//
// Created by Sam Bock on 9/20/16.
//
#include <conf/AxmParser.h>
#include "VoxelCoord.h"

void operator>>(const AxmNode &node, glm::ivec3 &v) {
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
			Noto::error("Attempting to fill 3d coordinate from non-array, non-object: {}", (int)node.type);
		}
	}
}
