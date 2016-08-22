//
// Created by nvt on 8/5/16.
//



#include <catch.hpp>
#include <engine/Engine.h>
#include <Noto.h>

NEW_EVENT_TYPE(TestEvent) {
public:
    const int value;

    TestEvent(int value) : Event(TestEventType), value(value) {}
};

class TestComponent : public GameComponent {
public:
    int testValue = 1;
    int updateCount = 0;

    TestComponent(GameEngine *gameEngine) : GameComponent(gameEngine) {

    }

    virtual void init() override {
        gameBus.onEvent<TestEvent>([&](TestEventPtr e) {
           testValue = e->value;
        });
    }

    virtual void update(Time dt) override {
        updateCount++;
    }

    virtual Time updateInterval() override {
        return Milliseconds(8);
    }
};

TEST_CASE("Game engine threading","[engine][concurrency]") {
    GameEngine engine;

    auto comp = engine.addComponent<TestComponent>();

    REQUIRE(engine.components.size() == 1);
    REQUIRE(engine.components[0] == comp);
    REQUIRE(comp->testValue == 1);

    engine.start();
    this_thread::sleep_for(chrono::milliseconds(250));
    int startCount = comp->updateCount;

    this_thread::sleep_for(chrono::milliseconds(1000));

    engine.endSignal = true;
    engine.exit();

    REQUIRE((comp->updateCount - startCount) > (int)((1000/8) * 0.40));
    REQUIRE((comp->updateCount - startCount) < (int)((1000/8) * 1.00));
}

TEST_CASE("gameEngineEventBus","[engine][event]") {
    GameEngine engine;

    auto comp = engine.addComponent<TestComponent>();

    REQUIRE(comp->testValue == 1);

    engine.eventBus.addEvent(TestEvent(9));

    REQUIRE(comp->testValue == 1);

    comp->updateRaw(Seconds(1));

    REQUIRE(comp->testValue == 9);
}

