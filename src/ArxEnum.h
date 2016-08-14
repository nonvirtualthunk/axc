
//
// Created by nvt on 8/3/16.
//

#ifndef TEST2_ARXENUM_H
#define TEST2_ARXENUM_H


#include <string>
#include <atomic>
#include <vector>
#include <mutex>
#include <map>
#include <containers/ArxMap.h>

class ArxEnum {
public:
    std::string name;
    int typeId;
    int ordinal;

    ArxEnum(const std::string &name, int typeId);

    bool operator==(const ArxEnum &rhs) const;

    bool operator!=(const ArxEnum &rhs) const;

    bool operator<(const ArxEnum &rhs) const;

    size_t hash() const {
        return (size_t) (ordinal * 31L + typeId);
    }
};


class ArxEnumData {
protected:
    static Arx::Map<int, ArxEnumData *> enumData;
    static std::mutex mutex;
    std::atomic_int creationCounter;

public:

    ArxEnumData() : creationCounter(0) {}

    static int nextOrdinalFor(int typeId) {
        std::lock_guard<std::mutex> guard(mutex);

        auto data = enumData.getOrElseUpdate(typeId, []() { return new ArxEnumData(); });

        int nextId = data->creationCounter.fetch_add(1);
        return nextId;
    }


};


#endif //TEST2_ARXENUM_H
