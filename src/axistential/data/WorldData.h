//
// Created by Sam Bock on 9/21/16.
//

#ifndef TEST2_TERRAIN_H
#define TEST2_TERRAIN_H


#include <core/AuxData.h>
#include <containers/TaleaGrid.h>
#include <graphics/Color.h>

typedef uint16_t MatVoxel;
typedef TaleaGrid<MatVoxel,true,2048> MaterialGrid;
typedef MaterialGrid::TaleaType MaterialTalea;

DECL_AUX_DATA(WorldAuxData, Terrain) {
public:
	MaterialGrid material;

	static constexpr uint16_t OpaqueMask =			0b1000000000000000;
	static constexpr uint16_t SolidMask =			0b0100000000000000;
	static constexpr uint16_t FluidBlockingMask =	0b0010000000000000;
	static constexpr uint16_t PartialOpaqueMask =	0b0001000000000000;

	static constexpr uint16_t IndexMask =			0b0000111111111111;

	static inline bool isSolid(uint16_t m) { return (m & SolidMask) == SolidMask; }
	static inline bool isOpaque(uint16_t m) { return (m & OpaqueMask) == OpaqueMask; }
	static inline bool isFluidBlocking(uint16_t m) { return (m & FluidBlockingMask) == FluidBlockingMask; }
	static inline bool isPartiallyOpaque(uint16_t m) { return (m & PartialOpaqueMask) == PartialOpaqueMask; }
	static inline bool isEntirelyTransparent(uint16_t m) { return (m & (OpaqueMask | PartialOpaqueMask)) == 0; }

public:
	Terrain_t() : material(0) {}
};

typedef int8_t LightVoxel;
typedef TaleaGrid<LightVoxel,false,2048> LightGrid;
typedef LightGrid::TaleaType LightTalea;

static constexpr LightVoxel MaxLight = 127;
static constexpr LightVoxel LightStep = MaxLight/31;
static constexpr LightVoxel OpaqueMarker = -1;

DECL_AUX_DATA(WorldAuxData, Light) {
public:
	LightGrid globalLightValues;
	Color globalLightColor = Color(255,255,255,255);
	float globalLightStrength = 1.0f;

public:
	Light_t() : globalLightValues(MaxLight) {}
};


#endif //TEST2_TERRAIN_H
