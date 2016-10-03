//
// Created by Sam Bock on 9/23/16.
//

#include <axistential/data/WorldData.h>
#include <math/AccumulatingStats.h>
#include <core/Metrics.h>
#include <axistential/graphics/Common.h>
#include "LightComponent.h"

void LightComponent::update(Time dt) {

}

LightComponent::LightComponent(GameEngine &gameEngine) : GameComponent(gameEngine) {}

void LightComponent::init() {
	Noto::info("Initializing lighting");
	auto& terrain = world(Terrain);
	auto& light = world(Light);

	MetricsTimer lightInitTimer("Light initialization timer");

	AccumVecRange3<VoxelCoord> coordRange(VoxelCoord(100000,100000,100000),VoxelCoord(-1000000,-1000000,-1000000));

	terrain.material.foreach([&](MaterialTalea *talea) {
		coordRange.apply(talea->position);
	});

	for (int tz = coordRange.max.z; tz >= coordRange.min.z; tz -= TaleaDim) {
		for (int ty = coordRange.min.y; ty <= coordRange.max.y; ty += TaleaDim) {
			for (int tx = coordRange.min.x; tx <= coordRange.max.x; tx += TaleaDim) {
				LightTalea& lightTal = light.globalLightValues.getTaleaRW(tx,ty,tz);
				lightTal.setAll(0);
			}
		}
	}

	for (int tz = coordRange.max.z + TaleaDim; tz >= coordRange.min.z - TaleaDim; tz -= TaleaDim) {
		for (int ty = coordRange.min.y - TaleaDim; ty <= coordRange.max.y + TaleaDim; ty += TaleaDim) {
			for (int tx = coordRange.min.x - TaleaDim; tx <= coordRange.max.x  + TaleaDim; tx += TaleaDim) {
				initializeLightingTalea(tx,ty,tz,terrain,light);
			}
		}
	}

	lightInitTimer.printElapsed();
}

struct InnerCoord {
	uint8_t x;
	uint8_t y;
	uint8_t z;
};

struct FloodPoint {
	int16_t x;
	int16_t y;
	int16_t z;
	LightVoxel lv;
};

//const glm::ivec3 cardinals[6] = {glm::ivec3(-1,0,0),glm::ivec3(0,-1,0),glm::ivec3(0,0,-1),glm::ivec3(1,0,0),glm::ivec3(0,1,0),glm::ivec3(0,0,1)};

constexpr int LightCardinalCount = 6;
constexpr const glm::ivec3* LightCardinals = cardinals;
void adjacentOf(int x, int y, int z, int q, int& ax, int& ay, int& az) {
	if (q == 0) {
		ax = x - 1;
		ay = y;
		az = z;
	} else if (q == 1) {
		ax = x;
		ay = y - 1;
		az = z;
	} else if (q == 2) {
		ax = x;
		ay = y;
		az = z - 1;
	} else if (q == 3) {
		ax = x + 1;
		ay = y;
		az = z;
	} else if (q == 4) {
		ax = x;
		ay = y + 1;
		az = z;
	} else if (q == 5) {
		ax = x;
		ay = y;
		az = z + 1;
	}
}

//#define IN_BOUNDS_LOCAL(xv,yv,zv) (((xv & TaleaMask) == xv) && ((yv & TaleaMask) == yv) && ((zv & TaleaMask) == zv))
#define IN_BOUNDS_LOCAL(xv,yv,zv) xv >= 0 && xv < TaleaDim && yv >= 0 && yv < TaleaDim && zv >= 0 && zv < TaleaDim

void LightComponent::initializeLightingTalea(int tx, int ty, int tz, Terrain_t &terrain, Light_t &light) {
	auto& aboveMat = terrain.material.getTaleaRO(tx,ty,tz+TaleaDim);
	auto& aboveLight = light.globalLightValues.getTaleaRO(tx,ty,tz+TaleaDim);

	auto& curMat = terrain.material.getTaleaRO(tx,ty,tz);

	// short circuit if there's full light above us and no obstructions in our current talea
	if (aboveLight.areAllDefault() && curMat.areAllDefault()) {
		return;
	}

	auto& curLight = light.globalLightValues.getTaleaRW(tx,ty,tz);
	// slice above the current slice. Initialized with the bottom-most slice of the talea above
	LightVoxel * aboveRow = new LightVoxel[TaleaDim];
	MatVoxel curMatRow[TaleaDim];
//	aboveLight.loadSlice(0,aboveSlice);

	std::deque<InnerCoord> localFillPoints;

	for (int z = TaleaDim-1; z >= 0; --z) {
		for (int y = 0; y < TaleaDim; ++y) {
			if (z == TaleaDim-1) {
				aboveLight.loadRow(y,0,aboveRow);
			} else {
				curLight.loadRow(y,z+1, aboveRow);
			}
			LightVoxel* curRow = curLight.writeableRow(y, z);
			curMat.loadRow(y,z,curMatRow);
			for (int x = 0; x < TaleaDim; ++x) {
				LightVoxel& cur = curRow[x];
				if (Terrain_t::isOpaque(curMatRow[x])) {
					cur = OpaqueMarker;
				} else {
					const LightVoxel above = aboveRow[x];
					if (above != MaxLight) {
						// if moving from an opaque to a clear, or if this is at the edge, add it to local fill points
						// for a flood out
						if (above == OpaqueMarker || z == TaleaDim - 1) {
							localFillPoints.push_back({(uint8_t) x, (uint8_t) y, (uint8_t) z});
						} else {
							cur = (LightVoxel) (std::max(above, (LightVoxel)LightStep) - LightStep);
						}
					} else {
						cur = MaxLight;
					}
				}
			}
		}
	}

	std::deque<FloodPoint> globalFloodPoints;

	LightVoxel adjacents[LightCardinalCount];
	while (!localFillPoints.empty()) {
		auto coord = localFillPoints.back();
		localFillPoints.pop_back();

		int gx = tx + coord.x, gy = ty + coord.y, gz = tz + coord.z;
		const LightVoxel cur = curLight(coord);
		LightVoxel maxAdj = 0;
		for (int q = 0; q < LightCardinalCount; ++q) {
			const glm::ivec3& cardinal = LightCardinals[q];
			int agx = gx + cardinal.x, agy = gy + cardinal.y, agz = gz + cardinal.z;
			const LightVoxel adjV = light.globalLightValues(agx,agy,agz);
			adjacents[q] = adjV;
			const LightVoxel lv = adjV - LightStep;
			maxAdj = std::max(maxAdj, lv);
		}

		if (cur < maxAdj) {
			curLight.set(coord,maxAdj);

			for (int q = 0; q < LightCardinalCount; ++q) {
				// if the q'th adjacent is less than what our current voxel could propagate to it, then we need to add
				// it to the queue, assuming that it is within the current talea
				if (adjacents[q] != OpaqueMarker && adjacents[q] < maxAdj - LightStep) {
					const glm::ivec3& cardinal = LightCardinals[q];
					int lx = coord.x + cardinal.x, ly = coord.y + cardinal.y, lz = coord.z + cardinal.z;
					// if it is within the bounds of the current talea
					if (IN_BOUNDS_LOCAL(lx,ly,lz)) {
						localFillPoints.push_front({(uint8_t) lx, (uint8_t) ly, (uint8_t) lz});
					} else if (lz >= TaleaDim){
						globalFloodPoints.push_back({(int16_t) (gx + cardinal.x), (int16_t) (gy + cardinal.y), (int16_t) (gz + cardinal.z), LightVoxel(maxAdj - LightStep)});
					}
				}
			}
		}
	}

	while (!globalFloodPoints.empty()) {
		const FloodPoint& flood = globalFloodPoints.front();
		if (!Terrain_t::isOpaque(terrain.material(flood))) {
			LightTalea& tal = light.globalLightValues.getTaleaRW(flood);
			if (tal.setIfGreater(flood.x - tal.x(),flood.y - tal.y(),flood.z - tal.z(), flood.lv)) {
				if (flood.lv > LightStep) {
					for (int q = 0; q < LightCardinalCount; ++q) {
						const glm::ivec3& card = LightCardinals[q];
						globalFloodPoints.push_back({(int16_t) (flood.x + card.x), (int16_t) (flood.y + card.y),
													 (int16_t) (flood.z + card.z), (LightVoxel) (flood.lv - LightStep) });
					}
				}
			}
		}

		globalFloodPoints.pop_front();
	}
}
