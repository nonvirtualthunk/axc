//
// Created by Sam Bock on 8/24/16.
//

#ifndef TEST2_MSDFTESTBED_H
#define TEST2_MSDFTESTBED_H


#include <application/GameApplication.h>
#include "Testbed.h"

class MSDFTestbed : public Testbed {
public:
    MSDFTestbed(const string &name);

public:
    virtual void init(GameApplication& app, GameEngine &gameEngine, GraphicsEngine &graphicsEngine, ControlEngine &controlEngine) override;

    static MSDFTestbed * instance;
};


#endif //TEST2_MSDFTESTBED_H
