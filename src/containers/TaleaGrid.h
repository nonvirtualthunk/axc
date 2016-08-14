//
// Created by Sam Bock on 8/9/16.
//

#ifndef TEST2_TALEAGRID_H
#define TEST2_TALEAGRID_H

#include <mutex>
#include <core/Predef.h>
#include <unordered_set>
#include "Talea.h"

template<typename T, bool onlyTrackDelta, int CS>
class TaleaGrid {
protected:
    typedef Talea<T, onlyTrackDelta> TaleaType;
    typedef TaleaType* TaleaPtr;
    typedef Arx::Map<VoxelCoord,TaleaType*> TaleaMap;

    const int CoreDimPo2 = pow2roundup(CS);
    const int CoreDimPo2TalShift = CoreDimPo2 - TaleaDimPo2;
    constexpr static int CoreDim = CS;
    constexpr static int CoreDim2 = CoreDim * 2;
    constexpr static int ToPositive = 1 << 20;
    constexpr static int ToPositiveDownshifted = ToPositive >> TaleaDimPo2;
    constexpr static int CoreMin = -CS;
    constexpr static int CoreMax = +CS;

    std::mutex outOfMainMutex;
    T sentinel;
    TaleaType* sentinelTalea = new TaleaType(sentinel,VoxelCoord(-10000,-10000,-10000));
    TaleaMap nonCoreTaleae;
    std::unordered_set<uint16_t> nonSentinelIndices;

    std::atomic<TaleaType*> coreTaleae[CoreDim * CoreDim];
public:
    TaleaGrid(T sentinel) : sentinel(sentinel) {
        for (int i = 0; i < CoreDim*CoreDim; ++i) {
            coreTaleae[i].store(nullptr, std::memory_order_relaxed);
        }
        sentinelTalea->isSentinel = true;
    }

    const TaleaType& getTaleaRO(int x, int y, int z) const {
        int ax = x + ToPositive;
        int ay = y + ToPositive;
        int az = z + ToPositive;

        int sx = (ax >> TaleaDimPo2) - ToPositiveDownshifted;
        int sy = (ay >> TaleaDimPo2) - ToPositiveDownshifted;
        int sz = (az >> TaleaDimPo2) - ToPositiveDownshifted;

        if (ax >= CoreMin && ay >= CoreMin && az >= CoreMin && ax < CoreMax && ay < CoreMax && az < CoreMax) {
            int index = (sz << (CoreDimPo2TalShift + CoreDimPo2TalShift)) + (sy << CoreDimPo2TalShift) + sx;

            const std::atomic<TaleaType*>& at_tal = coreTaleae[index];
            TaleaType* rawTal = at_tal.load(std::memory_order_relaxed);

            if (rawTal == nullptr) {
                return *sentinelTalea;
            } else {
                return *rawTal;
            }
        } else {
            // TODO: maybe not necessary?
//            std::lock_guard<std::mutex> guard(outOfMainMutex);

            auto coord = VoxelCoord(sx,sy,sz) * (1 << TaleaDimPo2);
            return *nonCoreTaleae.getOrElse(coord, sentinelTalea);
        }
    }

    TaleaType& getTaleaRW(int x, int y, int z) {
        int ax = x + ToPositive;
        int ay = y + ToPositive;
        int az = z + ToPositive;

        int sx = (ax >> TaleaDimPo2) - ToPositiveDownshifted;
        int sy = (ay >> TaleaDimPo2) - ToPositiveDownshifted;
        int sz = (az >> TaleaDimPo2) - ToPositiveDownshifted;

        // Index shifting is off

        if (ax >= CoreMin && ay >= CoreMin && az >= CoreMin && ax < CoreMax && ay < CoreMax && az < CoreMax) {
            int index = (sz << (CoreDimPo2TalShift + CoreDimPo2TalShift)) + (sy << CoreDimPo2TalShift) + sx;

            std::atomic<TaleaType*>& at_tal = coreTaleae[index];
            TaleaType* rawTal = at_tal.load(std::memory_order_relaxed);

            if (rawTal == nullptr) {
                nonSentinelIndices.emplace((uint16_t)index);
                TaleaType * newTal = new TaleaType(sentinel, VoxelCoord((glm::ivec3(sx,sy,sz) * (1 << TaleaDimPo2))));
                if (!at_tal.compare_exchange_strong(rawTal, newTal)) {
                    delete newTal;
                    return *at_tal.load();
                } else {
                    return *newTal;
                }
            } else {
                return *rawTal;
            }
        } else {
            std::lock_guard<std::mutex> guard(outOfMainMutex);

            auto coord = VoxelCoord((glm::ivec3(sx,sy,sz) * (1 << TaleaDimPo2)));
            return *nonCoreTaleae.getOrElseUpdate(coord, [&](){ return new TaleaType(sentinel, coord);});
        }
    }

    inline const T& operator()(const VoxelCoord& v) const {
        return operator()(v.x,v.y,v.z);
    }
    inline const T& operator()(int x, int y, int z) const {
        const TaleaType& tal = getTaleaRO(x,y,z);
        if (tal.isSentinel) {
            return sentinel;
        } else {
            int rx = x - tal.position.x;
            int ry = y - tal.position.y;
            int rz = z - tal.position.z;

            return tal(rx,ry,rz);
        }
    }

    inline void set(int x,int y,int z, const T& v) {
        TaleaType& tal = getTaleaRW(x,y,z);

        int rx = x - tal.position.x;
        int ry = y - tal.position.y;
        int rz = z - tal.position.z;

        tal.set(rx,ry,rz, v);
    }


    class Iterator {
        std::unordered_set<uint16_t>::const_iterator coreIndexIterator;
        std::unordered_set<uint16_t>::const_iterator coreIndexIteratorEnd;
        typename Arx::Map<VoxelCoord,TaleaType*>::ConstIteratorType nonCoreIterator;
        typename Arx::Map<VoxelCoord,TaleaType*>::ConstIteratorType nonCoreIteratorEnd;
        const TaleaGrid<T,onlyTrackDelta,CS>& grid;

    public:
        Iterator(const TaleaGrid<T,onlyTrackDelta,CS>& grid)
                : coreIndexIterator(grid.nonSentinelIndices.begin()),
                  coreIndexIteratorEnd(grid.nonSentinelIndices.end()),
                  nonCoreIterator(grid.nonCoreTaleae.begin()),
                  nonCoreIteratorEnd(grid.nonCoreTaleae.end()),
                  grid(grid) {}


        TaleaType* next() {
            if (coreIndexIterator != coreIndexIteratorEnd) {
                TaleaType* ret = grid.coreTaleae[*coreIndexIterator];
                ++coreIndexIterator;
                return ret;
            } else if (nonCoreIterator != nonCoreIteratorEnd) {
                TaleaType* ret = nonCoreIterator->second;
                ++nonCoreIterator;
                return ret;
            } else {
                return nullptr;
            }
        }

        Optional<TaleaType*> nextOpt() {
            TaleaType* ret = next();
            if (ret == nullptr) {
                return none<TaleaType*>();
            } else {
                return some(ret);
            }
        }

        bool hasNext() const {
            return coreIndexIterator != coreIndexIteratorEnd || nonCoreIterator != nonCoreIteratorEnd;
        }
    };

    Iterator iter() const {
        return Iterator(*this);
    }

};

template<typename T, bool onlyTrackDelta, int CS>
const int TaleaGrid<T,onlyTrackDelta,CS>::ToPositive;

#endif //TEST2_TALEAGRID_H
