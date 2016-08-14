//
// Created by nvt on 8/8/16.
//

#include "GameApplication.h"


GameApplication::GameApplication() :
graphicsEngine(&gameEngine),
controlEngine(&gameEngine,&graphicsEngine)
{

}

void GameApplication::onEvent(std::shared_ptr<Event> e) {
    controlEngine.eventBus.addEventPtr(e);
}

void GameApplication::update(Time dt) {
    // do nothing
}

void GameApplication::draw() {
    graphicsEngine.draw();
}


void GameApplication::init() {
    Application::init();

    gameEngine.init();
    graphicsEngine.init();
    controlEngine.init();
}

void GameApplication::loop() {
    gameEngine.start();
    graphicsEngine.start();
    controlEngine.start();

    Application::loop();
}

void GameApplication::shutdown() {
    controlEngine.exit();
    graphicsEngine.exit();
    gameEngine.exit();

    Application::shutdown();
}
