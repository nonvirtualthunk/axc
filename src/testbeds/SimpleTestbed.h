//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_SIMPLETESTBED_H
#define TEST2_SIMPLETESTBED_H


#include <engine/Engine.h>
#include "Testbed.h"

class SimpleTestbed : public Testbed {
public:
    SimpleTestbed(const string &name);

public:
    virtual void init(GameApplication& app, GameEngine &gameEngine, GraphicsEngine &graphicsEngine, ControlEngine &controlEngine) override;
};

#endif //TEST2_SIMPLETESTBED_H
