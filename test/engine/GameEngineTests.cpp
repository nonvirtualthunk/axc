//
// Created by nvt on 8/5/16.
//



#include <catch.hpp>
#include <engine/Engine.h>

class TestComponent : public GameComponent {
public:
    int testValue = 1;
    int updateCount = 0;

    TestComponent(GameEngine *gameEngine) : GameComponent(gameEngine) {

    }

    virtual void init() override {

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

    engine.endSignal = true;
    engine.exit();

    REQUIRE(comp->updateCount > (int)((250/8) * 0.40));
    REQUIRE(comp->updateCount < (int)((250/8) * 1.00));
}