//
// Created by nvt on 8/5/16.
//

#ifndef TEST2_GAMEENGINE_H
#define TEST2_GAMEENGINE_H


#include <core/AuxData.h>
#include <thread>
#include <Event.h>

class World;

template<typename ComponentType>
class Engine {
public:
    int parallelism = 2;
    EventBus eventBus;
    std::vector<ComponentType *> components;
    std::vector<std::thread> threads;
    bool endSignal = false;
    atomic_int runningThreads;

    Engine() : runningThreads(0) {}

    template<typename C>
    C* addComponent() {
        typedef typename C::engine_type castTo;
        C *comp = new C((castTo*)this);
        components.push_back(comp);
        comp->init();
        return comp;
    }

    virtual void init(){

    }

    void start() {
        init();
        for (int i = 0; i < parallelism; ++i) {
            std::vector<ComponentType *> componentsForThread;
            for (int j = 0; j < components.size(); ++j) {
                if (j % parallelism == i) {
                    componentsForThread.push_back(components[j]);
                }
            }

            const std::vector<ComponentType *> lockedComps = componentsForThread;

            threads.push_back(std::thread([=](){
                for (ComponentType* comp : lockedComps) {
                    comp->lastUpdated = std::chrono::steady_clock::now();
                }
                while (!endSignal) {
                    auto started = std::chrono::steady_clock::now();
                    for (ComponentType* comp : lockedComps) {
                        auto cur = std::chrono::steady_clock::now();
                        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(cur - comp->lastUpdated);
                        Time dt = Milliseconds(delta.count());
                        if (dt > comp->updateInterval()) {
                            comp->updateRaw(dt);
                            comp->lastUpdated = cur;
                        }
                    }
                    std::this_thread::sleep_until(started + std::chrono::milliseconds(10));
                }
                runningThreads.fetch_sub(1);
            }));
            runningThreads.fetch_add(1);
        }
    }

    void exit() {
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
};

template<typename EngineType>
class EngineComponent {
public:
    typedef EngineType engine_type;
    std::chrono::steady_clock::time_point lastUpdated;



    virtual Time updateInterval() {
        return Milliseconds(8);
    }

    virtual void init() {}

    virtual void updateRaw(Time dt) {
        updateComp(dt);
    }

    virtual void updateComp(Time dt) = 0;

    virtual void update(Time dt) = 0;
};

class GameComponent;

class GameEngine : public Engine<GameComponent> {

public:
    World * world;

};

class GameComponent : public EngineComponent<GameEngine> {
protected:
    GameEngine *gameEngine;
    World * world;
    EventBus::Watcher gameBus;

public:
    GameComponent(GameEngine *gameEngine);

    virtual void updateComp(Time dt) override;
};


class GraphicsComponent;

class GraphicsEngine : public Engine<GraphicsComponent> {
public:
    GameEngine* gameEngine;

    GraphicsEngine(GameEngine *gameEngine);
};

class GraphicsComponent : public EngineComponent<GraphicsEngine> {
protected:
    GraphicsEngine* graphicsEngine;
    World* world;
    EventBus::Watcher gameBus;
    EventBus::Watcher graphicsBus;

public:
    GraphicsComponent(GraphicsEngine *graphicsEngine);

    virtual void draw() = 0;

    virtual void updateComp(Time dt) override;
};


#endif //TEST2_GAMEENGINE_H
