//
// Created by nvt on 8/5/16.
//

#ifndef TEST2_GAMEENGINE_H
#define TEST2_GAMEENGINE_H


#include <core/AuxData.h>
#include <thread>
#include <Event.h>
#include <Noto.h>

class World;
class WindowingSystem;

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
                    comp->lastUpdated = Nanoseconds(epochNanosSteady());
                }
                while (!endSignal) {
                    auto started = std::chrono::high_resolution_clock::now();
                    for (ComponentType* comp : lockedComps) {
                        auto cur = Nanoseconds(epochNanosSteady());
                        auto dt = cur - comp->lastUpdated;
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
        endSignal = true;
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
};

template<typename EngineType>
class EngineComponent {
public:
    typedef EngineType engine_type;
    Time lastUpdated = Seconds(0);


    virtual Time updateInterval() {
        return Milliseconds(8);
    }

    virtual void init() {}

    virtual void updateRaw(Time dt) {
        updateComp(dt);
    }

    virtual void updateComp(Time dt) = 0;

    virtual void update(Time dt) = 0;

    Time deltaSinceLastUpdate() {
        return lastUpdated - lastUpdated;
    }
};

class GameComponent;

class GameEngine : public Engine<GameComponent> {

public:
    World * world;

    GameEngine();

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

    void draw();
};

class GraphicsComponent : public EngineComponent<GraphicsEngine> {
protected:
    GraphicsEngine* graphicsEngine;
    World* world;
    EventBus::Watcher gameBus;
    EventBus::Watcher graphicsBus;

public:
    Time lastDrawn;

    GraphicsComponent(GraphicsEngine *graphicsEngine);

    virtual void draw() = 0;

    virtual void updateComp(Time dt) override;

    Time nextExpectedSwap() const;
};


class ControlComponent;

class ControlEngine : public Engine<ControlComponent> {
public:
    GameEngine* gameEngine;
    GraphicsEngine* graphicsEngine;
    WindowingSystem* windowingSystem;
    AuxDataContainer<ControlAuxData<void>> controlData;

    ControlEngine(GameEngine *gameEngine, GraphicsEngine *graphicsEngine);

    void draw();
};

class ControlComponent : public EngineComponent<ControlEngine> {
protected:
    ControlEngine* controlEngine;
    World* world;
    EventBus::Watcher gameBus;
    EventBus::Watcher graphicsBus;
    EventBus::Watcher controlBus;

public:
    Time lastDrawn;

    ControlComponent(ControlEngine *controlEngine);

    virtual void updateComp(Time dt) override;

    virtual void draw(){}

    Time nextExpectedSwap() const;

    template<typename T>
    typename T::associated_type &aux(T adt) {
        return controlEngine->controlData.aux(adt);
    }
};

#endif //TEST2_GAMEENGINE_H
