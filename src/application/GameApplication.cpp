//
// Created by nvt on 8/8/16.
//

#include "GameApplication.h"

#include "core/Predef.h"

GameApplication::GameApplication() :
graphicsEngine(&gameEngine),
controlEngine(&gameEngine,&graphicsEngine)
{

}

void GameApplication::onEvent(std::shared_ptr<Event> e) {
    controlEngine.eventBus.addEvent(e);
}

void GameApplication::update(Time dt) {
    // do nothing
}

void GameApplication::draw() {
    graphicsEngine.draw();
}
