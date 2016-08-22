//
// Created by Sam Bock on 8/17/16.
//

#ifndef TEST2_WINDOWINGSYSTEMTESTBED_H
#define TEST2_WINDOWINGSYSTEMTESTBED_H


#include "Testbed.h"

class WindowingSystemTestbed : public Testbed {
public:
    WindowingSystemTestbed(const string &name);

public:
    virtual void init(GameApplication& app, GameEngine &gameEngine, GraphicsEngine &graphicsEngine, ControlEngine &controlEngine) override;

    static WindowingSystemTestbed * instance;
};

#endif //TEST2_WINDOWINGSYSTEMTESTBED_H
