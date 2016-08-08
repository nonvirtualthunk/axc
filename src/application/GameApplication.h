//
// Created by nvt on 8/8/16.
//

#ifndef TEST2_GAMEAPPLICATION_H
#define TEST2_GAMEAPPLICATION_H


#include <engine/Engine.h>
#include "Application.h"

class GameApplication : public Application {
public:
    GameEngine gameEngine;
    GraphicsEngine graphicsEngine;
    ControlEngine controlEngine;

    GameApplication();

protected:
    virtual void onEvent(std::shared_ptr<Event> e) override;

    virtual void update(Time dt) override;

    virtual void draw() override;
};


#endif //TEST2_GAMEAPPLICATION_H