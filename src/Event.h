//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_EVENT_H
#define TEST2_EVENT_H

#include <set>
#include <string>
#include "ArxEnum.h"
#include "UnitOfMeasure.h"
#include "Optional.h"
#include <vector>
#include <mutex>
#include <deque>
#include <core/Predef.h>

using namespace std;

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

class EventBus {
public:
    typedef std::function<void(EventPtr)> listenerFunc;
    std::mutex mutex;
    std::deque<EventPtr> events;
    Time keepEventsFor = Seconds(30);

    std::function<long long int()> timeFunction;

public:
    EventBus();

protected:
    void addEventPtr(EventPtr event);

public:

    template<typename E, typename std::enable_if<std::is_base_of<Event,E>::value>::type* = nullptr>
    void addEvent(const E& event) {
        auto newPtr = std::shared_ptr<E>(new E(event));
        newPtr->entryTime = timeFunction();
        addEventPtr(newPtr);
    }


    class AsyncIterator {
    protected:
        EventBus& bus;
        int mostRecentId = 0;
        int mostRecentIndex = 0;

    public:
        Optional<EventPtr> next();

        AsyncIterator(EventBus& bus) : bus(bus) {}
    };

    class Watcher {
    protected:
        std::unordered_multimap<EventType,listenerFunc,Arx::DelegatingHasher<EventType>> typedListeners;
        AsyncIterator iter;
    public:
        Watcher(AsyncIterator iter) : iter(iter) {}

        template<typename E, typename std::enable_if<std::is_base_of<Event,E>::value>::type* = nullptr>
        void onEvent(std::function<void(std::shared_ptr<E>)> func) {
            long typeId = getTypeId<E>();

            EventType correspondingEventType = EventType::getEventTypesByTypeId().get(typeId).get();
            listenerFunc tmpFunc = [func](EventPtr e) {
                func(static_pointer_cast<E>(e));
            };
            typedListeners.insert(std::pair<EventType,listenerFunc>(correspondingEventType,tmpFunc));
        }

        void update() {
            while (true) {
                auto ne = iter.next();
                if (ne.isEmpty()) {
                    break;
                } else {
                    auto event = ne.get();

                    auto endPoint = typedListeners.end();
                    for (auto listenIter = typedListeners.find(event->eventType); listenIter != endPoint; ++listenIter) {
                        listenIter->second(event);
                    }
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
