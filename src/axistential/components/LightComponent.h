//
// Created by Sam Bock on 9/23/16.
//

#ifndef TEST2_LIGHTCOMPONENT_H
#define TEST2_LIGHTCOMPONENT_H


#include <engine/Engine.h>

class Terrain_t;
class Light_t;

class LightComponent : public GameComponent {
public:
	LightComponent(GameEngine &gameEngine);

	virtual void init() override;

	virtual void update(Time dt) override;

	void initializeLightingTalea(int tx, int ty, int tz, Terrain_t &terrain, Light_t &light);
};


#endif //TEST2_LIGHTCOMPONENT_H
