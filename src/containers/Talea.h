//
// Created by Sam Bock on 8/8/16.
//

#ifndef TEST2_TALEA_H
#define TEST2_TALEA_H

#include <algorithm>
#include <glm/vec3.hpp>
#include <core/VoxelCoord.h>

constexpr static int TaleaDimPo2 = 5;
constexpr static int TaleaDimPo2_2 = TaleaDimPo2 * 2;
constexpr static int TaleaDim = 1 << TaleaDimPo2;

template<typename T, bool onlyTrackDelta>
class Talea {
public:
    struct Data {
        T* raw;
        int mask = 0x0000000;
    };
    T sentinel;
    std::atomic<Data *> data;
    int modificationCount = 0;
    bool isSentinel = false;
    VoxelCoord position;

    bool trackingEnabled = false;


public:
    Talea(T sentinel, const VoxelCoord& pos) : sentinel(sentinel), data(new Data()), position(pos) {
        data.load(std::memory_order_relaxed)->raw = new T[1];
        data.load(std::memory_order_relaxed)->raw[0] = sentinel;
    }

    virtual ~Talea() {
        delete[] data.load()->raw;
    }

public:

    template<typename V>
    inline const T& operator()(const V& vec) const {
        return operator()(vec.x,vec.y,vec.z);
    }

    inline const T& get(int x, int y,int z) const {
        return operator()(x,y,z);
    }
    inline const T& operator()(int x, int y,int z) const {
        int index = ((z << TaleaDimPo2_2) + (y << TaleaDimPo2) + x) & data.load(std::memory_order_relaxed)->mask;
        return data.load(std::memory_order_relaxed)->raw[index];
    }

    template<typename V>
    void set(const V& vec, const T& newValue) {
        set(vec.x,vec.y,vec.z,newValue);
    }

    void set(const int x, const int y,const int z, const T& newValue) {
        const int index = ((z << TaleaDimPo2_2) + (y << TaleaDimPo2) + x);
        Data* startData = data.load(std::memory_order_relaxed);
        Data* finalData = startData;
        if (startData->mask == 0x00000000) {
            Data * newData = new Data();
            newData->raw = new T[TaleaDim * TaleaDim * TaleaDim];
            newData->mask = 0xffffffff;
            std::fill_n(newData->raw, TaleaDim * TaleaDim * TaleaDim, sentinel);
            if (!data.compare_exchange_strong(startData, newData)) {
                delete[] newData->raw;
                delete newData;
                finalData = data.load();
            } else {
                finalData = newData;
            }
        }

        if (onlyTrackDelta) {
            T& ref = finalData->raw[index];
            if (ref != newValue) {
                if (trackingEnabled) {
                    modificationCount++;
                }
                ref = newValue;
            }
        } else {
            finalData->raw[index] = newValue;
            if (trackingEnabled) {
                modificationCount++;
            }
        }
    }
};

#endif //TEST2_TALEA_H
