//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_TESTBED_H
#define TEST2_TESTBED_H


#include <engine/Engine.h>
#include <application/GameApplication.h>

class Testbed {
public:
    const std::string name;

    Testbed(const std::string &name);

    virtual void init(GameApplication& app, GameEngine& gameEngine, GraphicsEngine& graphicsEngine, ControlEngine& controlEngine) = 0;

    static Arx::Map<std::string,Testbed*>& getTestbedsByName();
};


#endif //TEST2_TESTBED_H
