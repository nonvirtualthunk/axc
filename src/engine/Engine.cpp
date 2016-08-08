//
// Created by nvt on 8/5/16.
//

#include "Engine.h"

#include "core/Predef.h"

GraphicsComponent::GraphicsComponent(GraphicsEngine *graphicsEngine) :
        graphicsEngine(graphicsEngine),
        world(graphicsEngine->gameEngine->world),
        gameBus(graphicsEngine->gameEngine->eventBus.watcher()),
        graphicsBus(graphicsEngine->eventBus.watcher()) {}

void GraphicsComponent::updateComp(Time dt) {
    gameBus.update();
    graphicsBus.update();

    update(dt);
}

GraphicsEngine::GraphicsEngine(GameEngine *gameEngine) : gameEngine(gameEngine) {}

void GraphicsEngine::draw() {
    for (GraphicsComponent* comp : components) {
        comp->draw();
    }
}

GameComponent::GameComponent(GameEngine *gameEngine) :
        gameEngine(gameEngine),
        world(gameEngine->world),
        gameBus(gameEngine->eventBus.watcher()) {

}

void GameComponent::updateComp(Time dt) {
    gameBus.update();

    update(dt);
}

ControlComponent::ControlComponent(ControlEngine *controlEngine) :
        controlEngine(controlEngine),
        world(controlEngine->graphicsEngine->gameEngine->world),
        controlBus(controlEngine->eventBus.watcher()),
        gameBus(controlEngine->gameEngine->eventBus.watcher()),
        graphicsBus(controlEngine->graphicsEngine->eventBus.watcher()) {}

{

}

void ControlComponent::updateComp(Time dt) {
    gameBus.update();
    graphicsBus.update();
    controlBus.update();

    update(dt);
}

ControlEngine::ControlEngine(GameEngine *gameEngine, GraphicsEngine *graphicsEngine) :
        gameEngine(gameEngine),
        graphicsEngine(graphicsEngine)
{

}
