//
// Created by nvt on 8/3/16.
//

#include <glm/vec2.hpp>
#include <vector>
#include <Event.h>
#include <catch.hpp>
#include <GUIEvents.h>
#include <core/Predef.h>


TEST_CASE ("Event creation","[event]") {
    auto e = MouseButtonEvent(glm::vec2(1.0f,1.0f), true, KeyModifiers(0));

    REQUIRE(e.eventType == MouseButtonEventType);

    auto e2 = MouseButtonEvent(glm::vec2(2.0f,0.0f), false, KeyModifiers(0));

    REQUIRE(e.id != e2.id);

    auto e3 = KeyEvent(1,true, KeyModifiers(0));

    REQUIRE(e3.id != e.id);
    REQUIRE(e3.eventType != e2.eventType);
}

TEST_CASE("Event bus async iterator","[event]") {
    auto e1 = MouseButtonEvent(glm::vec2(1.0f,1.0f), false, KeyModifiers(0));
    auto e2 = MouseButtonEvent(glm::vec2(0.0f,0.0f), true, KeyModifiers(0));

    EventBus bus;

    bus.addEvent(e1);
    bus.addEvent(e2);

    REQUIRE(bus.events.size() == 2);

    auto iter = bus.iter();

    auto firstOff = iter.next();
    REQUIRE(firstOff.isPresent());
    REQUIRE(firstOff.get()->eventType == e1.eventType);
    REQUIRE(firstOff.get()->id == e1.id);

    auto secondOff = iter.next();
    REQUIRE(secondOff.isPresent());
    REQUIRE(secondOff.get()->id == e2.id);

    auto thirdOff = iter.next();
    REQUIRE(thirdOff.isEmpty());
}

TEST_CASE("Event bus age out","[event]") {
    auto e1 = MouseButtonEvent(glm::vec2(1.0f,1.0f), false, KeyModifiers(0));
    auto e2 = MouseButtonEvent(glm::vec2(0.0f,0.0f), true, KeyModifiers(0));

    int curTime = 1;

    EventBus bus;
    bus.timeFunction = [&]() { return curTime; };
    bus.keepEventsFor = Seconds(1);

    bus.addEvent(e1);
    bus.addEvent(e2);

    REQUIRE(bus.events.size() == 2);

    auto iter = bus.iter();
    REQUIRE(iter.next().get()->id == e1.id);

    curTime += Seconds(10).in(Milliseconds);

    auto e3 = MouseButtonEvent(glm::vec2(4.0f,0.0f), true, KeyModifiers(0));

    bus.addEvent(e3);

    REQUIRE(bus.events.size() == 1);

    auto afterFetch = iter.next();

    REQUIRE(afterFetch.isPresent());
    REQUIRE(afterFetch.get()->id == e3.id);
}

TEST_CASE("Event bus listeners","[event]") {
    auto e1 = MouseButtonEvent(glm::vec2(1.0f,1.0f), false, KeyModifiers(0));
    auto e2 = KeyEvent(1, true, KeyModifiers(0));

    EventBus bus;
    auto watcher = bus.watcher();

    bool mouseButtonSeen = false;
    bool keySeen = false;
    watcher.onEvent<MouseButtonEvent>([&](MouseButtonEventPtr e) { mouseButtonSeen = true; });
    watcher.onEvent<KeyEvent>([&](KeyEventPtr e) { keySeen = true; });

    bus.addEvent(e1);
    watcher.update();

    REQUIRE(mouseButtonSeen);
    REQUIRE(!keySeen);

    bus.addEvent(e2);
    watcher.update();

    REQUIRE(mouseButtonSeen);
    REQUIRE(keySeen);
}