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

GameComponent::GameComponent(GameEngine *gameEngine) :
        gameEngine(gameEngine),
        world(gameEngine->world),
        gameBus(gameEngine->eventBus.watcher()) {

}

void GameComponent::updateComp(Time dt) {
    gameBus.update();

    update(dt);
}
