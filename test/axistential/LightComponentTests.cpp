//
// Created by Sam Bock on 9/26/16.
//



#include <catch.hpp>
#include <axistential/components/LightComponent.h>
#include <axistential/data/WorldData.h>
#include <math/Shapes3d.h>

const static MatVoxel vox = 1 | Terrain_t::OpaqueMask;

TEST_CASE("LightComponentInitializationBasicCase","[lighting][axis]") {
	World world;
	GameEngine engine(world);
	LightComponent lightComp(engine);

	auto& light = world(Light);
	auto& terrain = world(Terrain);

	Cube<int>({-10,-10,-10}, {10,10,-10}).foreach([&terrain](int x,int y,int z){
		terrain.material.set(x,y,z, vox);
	});

	Cube<int>({-5,-5,5},{5,5,5}).foreach([&terrain](int x,int y,int z) {
		terrain.material.set(x,y,z, vox);
	});

	lightComp.init();

	const auto& lvs = light.globalLightValues;
	REQUIRE(lvs(0,0,-10) == OpaqueMarker);
	REQUIRE(terrain.material(0,0,-9) == 0);
	REQUIRE(lvs(0,0,-9) != OpaqueMarker);
	REQUIRE(lvs(0,0,10) == MaxLight);
	REQUIRE(lvs(0,0,0) != MaxLight);

	for (int dx = 0; dx < 5; ++dx) {
		for (int z = -9; z < 5; ++z) {
			// stepping inward from the middle
			REQUIRE(int(lvs(-5 + dx,0,z)) == MaxLight - LightStep * (dx + 1));
			REQUIRE(int(lvs(5 - dx,0,z)) == MaxLight - LightStep * (dx + 1));
			// stepping along the edge
			REQUIRE(int(lvs(-5 + dx,-5,z)) == MaxLight - LightStep);
		}
	}
}

TEST_CASE("LightComponentInitializationAdvancedFlowCase","[lighting][axis]") {
	World world;
	GameEngine engine(world);
	LightComponent lightComp(engine);

	auto& light = world(Light);
	auto& terrain = world(Terrain);

	std::function<void(int,int,int)> setVox = [&terrain](int x,int y,int z){
		terrain.material.set(x,y,z, vox);
	};
	Cube<int>({-10,-10,-10}, {10,10,-10}).foreach(setVox);
	Cube<int>({-5,-5,0},{5,5,0}).foreach(setVox);
	Cube<int>({7,-5,-2},{7,5,10}).foreach(setVox);
	Cube<int>({7,-5,10},{15,5,10}).foreach(setVox);
	Cube<int>({7,-5,-2},{15,-5,10}).foreach(setVox);
	Cube<int>({7,5,-2},{15,5,10}).foreach(setVox);

	lightComp.init();

	const auto& lvs = light.globalLightValues;
	REQUIRE(int(lvs(6,0,0)) == MaxLight);
	REQUIRE(int(lvs(7,0,-2)) == OpaqueMarker);
	REQUIRE(int(lvs(6,0,-3)) == MaxLight);
	REQUIRE(int(lvs(7,0,-3)) == MaxLight - LightStep);
	REQUIRE(int(lvs(8,0,-3)) == MaxLight - LightStep * 2);
	REQUIRE(int(lvs(8,0,-2)) == MaxLight - LightStep * 3); // basically it has to go under the wall, then back up, 3 steps
}