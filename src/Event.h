//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_EVENT_H
#define TEST2_EVENT_H

#include <string>
#include "ArxEnum.h"
#include "UnitOfMeasure.h"
#include <vector>
#include <mutex>
#include <deque>
#include <core/Predef.h>

using namespace std;

template<typename T>
class Optional;

class EventType : public ArxEnum {
public:
    EventType(const EventType& other) = default;
    EventType(const std::string &name, int associatedTypeId);

public:
    static Arx::Map<long,EventType>& getEventTypesByTypeId();
    static Arx::Map<std::string,EventType>& getEventTypesByName();
    static EventType getOrCreateEventType(std::string name, int associatedTypeId);
};

static atomic_int EventIdCounter(1);

class Event {
public:
    const EventType &eventType;
    decltype(chrono::steady_clock::now()) creationTime = chrono::steady_clock::now();
    long long int entryTime = 0L;
    const int id = EventIdCounter.fetch_add(1);
    bool consumed = false;

    void consume() {
        consumed = true;
    }
protected:
    Event(const EventType &eventType) : eventType(eventType) {}
};

#define NEW_EVENT_TYPE(TNAME) \
class TNAME; \
using TNAME ## Ptr = std::shared_ptr<TNAME>; \
const static EventType TNAME ## Type = EventType::getOrCreateEventType(#TNAME, getTypeId<TNAME>()); \
\
class TNAME : public Event



using EventPtr = std::shared_ptr<Event>;



class EventResponder {
public:
    typedef std::function<void(EventPtr)> listenerFunc;
protected:
    std::unordered_multimap<EventType,listenerFunc,Arx::DelegatingHasher<EventType>> typedListeners;
public:
    template<typename E, typename std::enable_if<std::is_base_of<Event,E>::value>::type* = nullptr>
    void onEvent(std::function<void(std::shared_ptr<E>)> func) {
        long typeId = getTypeId<E>();

        EventType correspondingEventType = EventType::getEventTypesByTypeId().get(typeId).get();
        listenerFunc tmpFunc = [func](EventPtr e) {
            func(static_pointer_cast<E>(e));
        };
        typedListeners.insert(std::pair<EventType,listenerFunc>(correspondingEventType,tmpFunc));
    }

    template<typename E, typename std::enable_if<std::is_base_of<Event,E>::value>::type* = nullptr>
    void handle(std::shared_ptr<E> event ) {
        auto endPoint = typedListeners.end();
        auto listenIter = typedListeners.find(event->eventType);
        for (; listenIter != endPoint; ++listenIter) {
            listenIter->second(event);
        }
    }
};

class EventBus {
public:
    std::mutex mutex;
    std::deque<EventPtr> events;
    Time keepEventsFor = Seconds(10);

    std::function<long long int()> timeFunction;

public:
    EventBus();

public:

    template<typename E, typename std::enable_if<std::is_base_of<Event,E>::value>::type* = nullptr>
    void addEvent(const E& event) {
        auto newPtr = std::shared_ptr<E>(new E(event));
        addEventPtr(newPtr);
    }

    void addEventPtr(EventPtr event);


    class AsyncIterator {
    public:
        EventBus& bus;
        int mostRecentId = 0;
        int mostRecentIndex = 0;

    public:
        Optional<EventPtr> next();

        AsyncIterator(EventBus& bus) : bus(bus) {}
    };

    class Watcher : public EventResponder {
    protected:
        AsyncIterator iter;
    public:
        Watcher(AsyncIterator iter) : iter(iter) {}

        void update() {
            while (true) {
                auto ne = iter.next();
                if (ne.isEmpty()) {
                    break;
                } else {
                    auto event = ne.get();

                    handle(event);
                }
            }
        }
    };

    AsyncIterator iter() {
        return AsyncIterator(*this);
    }

    Watcher watcher() {
        return Watcher(iter());
    }
};

#endif //TEST2_EVENT_H
