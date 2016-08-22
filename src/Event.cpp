//
// Created by nvt on 8/3/16.
//

#include "Event.h"
#include "core/Predef.h"

Arx::Map<long,EventType>& EventType::getEventTypesByTypeId() {
    static Arx::Map<long,EventType> * map = new Arx::Map<long,EventType>();
    return *map;
};

Arx::Map<std::string,EventType>& EventType::getEventTypesByName() {
    static Arx::Map<std::string,EventType> * map = new Arx::Map<std::string,EventType>();
    return *map;
};

EventType::EventType(const std::string &name, int associatedTypeId) :
        ArxEnum(name, getTypeId<EventType>()) {
    EventType::getEventTypesByTypeId().put(associatedTypeId,*this);
    EventType::getEventTypesByName().put(name,*this);
}

EventType EventType::getOrCreateEventType(std::string name, int associatedTypeId) {
    auto existing = EventType::getEventTypesByName().get(name);
    if (existing.isPresent()) {
        return existing.get();
    } else {
        EventType ret(name,associatedTypeId);
        return ret;
    }
}

EventBus::EventBus() :
        timeFunction([]() { return epochMillisSteady(); })
{

}

void EventBus::addEventPtr(EventPtr event) {
    {

        std::lock_guard<std::mutex> guard(mutex);

        auto curTime = timeFunction();
        auto cutoffTime = curTime - (long) keepEventsFor.in(Milliseconds);
        event->entryTime = curTime;

        events.push_back(event);

        while (!events.empty() && events.front()->entryTime < cutoffTime) {
            events.pop_front();
        }
    }

}

Optional<EventPtr> EventBus::AsyncIterator::next() {
    std::lock_guard<std::mutex> guard(bus.mutex);

    while (mostRecentIndex > 0 &&
            bus.events.size() >= mostRecentIndex &&
           bus.events[mostRecentIndex-1]->id != mostRecentId) {
        mostRecentIndex--;
    }

    if (bus.events.size() > mostRecentIndex) {
        int idx = mostRecentIndex;
        mostRecentIndex++;
        mostRecentId = bus.events[idx]->id;
        return some(bus.events[idx]);
    } else {
        return none<EventPtr>();
    }
}