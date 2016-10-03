//
// Created by Sam Bock on 9/24/16.
//

#include "Common.h"


//	val low = 0.0f
//	val high = 1.0f
//	/* q,k order */
//	val cubePoints = Array[Array[ReadVec3f]](
//			Array[ReadVec3f]( ReadVec3f(low,high,high) , ReadVec3f(low,low,high)  , ReadVec3f(low,low,low), ReadVec3f(low,high,low)  ).reverse,  //Left
//			Array[ReadVec3f]( ReadVec3f(low,low,high) , ReadVec3f(high,low,high)  , ReadVec3f(high,low,low), ReadVec3f(low,low,low) ).reverse,   //Back
//			Array[ReadVec3f]( ReadVec3f(high,high,low) , ReadVec3f(low,high,low)  , ReadVec3f(low,low,low), ReadVec3f(high,low,low)  ).reverse,  //Bottom
//			Array[ReadVec3f]( ReadVec3f(high,low,high) , ReadVec3f(high,high,high), ReadVec3f(high,high,low), ReadVec3f(high,low,low)).reverse,  //Right
//			Array[ReadVec3f]( ReadVec3f(high,high,high) , ReadVec3f(low,high,high) , ReadVec3f(low,high,low), ReadVec3f(high,high,low) ).reverse,//Front
//			Array[ReadVec3f]( ReadVec3f(low,high,high), ReadVec3f(high,high,high) , ReadVec3f(high,low,high) , ReadVec3f(low,low,high) ).reverse //Top

constexpr float low = 0.0f;
constexpr float high = 1.0f;

const glm::vec3 cubePoints[6][4] = {
		{ glm::vec3(low,high,low),  glm::vec3(low,low,low), glm::vec3(low,low,high), glm::vec3(low,high,high) },  //Left
		{ glm::vec3(low,low,low), glm::vec3(high,low,low), glm::vec3(high,low,high),  glm::vec3(low,low,high) },   //Back
		{ glm::vec3(high,low,low), glm::vec3(low,low,low), glm::vec3(low,high,low), glm::vec3(high,high,low) },  //Bottom
		{ glm::vec3(high,low,low), glm::vec3(high,high,low), glm::vec3(high,high,high), glm::vec3(high,low,high) },  //Right
		{ glm::vec3(high,high,low), glm::vec3(low,high,low), glm::vec3(low,high,high), glm::vec3(high,high,high) },//Front
		{ glm::vec3(low,low,high), glm::vec3(high,low,high), glm::vec3(high,high,high), glm::vec3(low,high,high) } //Top
};

const glm::ivec3 cardinals[6] = {glm::ivec3(-1,0,0),glm::ivec3(0,-1,0),glm::ivec3(0,0,-1),glm::ivec3(1,0,0),glm::ivec3(0,1,0),glm::ivec3(0,0,1)};