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
constexpr static int TaleaMask = 0x0000001f;

template<typename T, bool onlyTrackDelta>
class Talea {
public:
    struct Data {
        T* raw;
        int mask = 0x0000000;
    };
    std::atomic<Data *> data;
    int modificationCount = 0;
    bool isSentinel = false;
    VoxelCoord position;

    bool trackingEnabled = false;

protected:
	T sentinel;

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

	void loadRow(int y, int z, T* target) const {
		if (data.load(std::memory_order_relaxed)->mask == 0x00000000) {
			std::fill(target, target + (TaleaDim), sentinel);
		} else {
			int index = ((z << TaleaDimPo2_2) + (y << TaleaDimPo2));
			Data * resolved = data.load(std::memory_order_relaxed);
			T * src = resolved->raw;
			std::copy(src + index, src + index + (TaleaDim), target);
		}
	}

	void loadSlice(int z, T* target) const {
		auto d = data.load(std::memory_order_relaxed);
		if (d->mask == 0x00000000) {
			std::fill(target, target + (TaleaDim * TaleaDim), sentinel);
		} else {
			int index = (z << TaleaDimPo2_2);
			T * src = d->raw;
			std::copy(src + index, src + index + (TaleaDim * TaleaDim), target);
		}
	}

protected:
	inline Data* getOrCreateData() {
		auto startData = data.load(std::memory_order_relaxed);
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
		return finalData;
	}
public:

	T* writeableRow(int y, int z) {
		Data* finalData = getOrCreateData();
		int index = (z << TaleaDimPo2_2) + (y << TaleaDimPo2);
		return finalData->raw + index;
	}

	T* writeableSlice(int z) {
		Data* finalData = getOrCreateData();
		int index = z << TaleaDimPo2_2;
		return finalData->raw + index;
	}

	bool areAllDefault() const {
		return data.load(std::memory_order_relaxed)->mask == 0x00000000;
	}

    template<typename V>
    void set(const V& vec, const T& newValue) {
        set(vec.x,vec.y,vec.z,newValue);
    }

	bool setIfGreater(const int x,const int y,const int z, const T& newValue) {
		if (get(x,y,z) < newValue) {
			set(x,y,z,newValue);
			return true;
		}
		return false;
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

        if (onlyTrackDelta && trackingEnabled) {
            T& ref = finalData->raw[index];
            if (ref != newValue) {
                modificationCount++;
                ref = newValue;
            }
        } else {
            finalData->raw[index] = newValue;
            if (trackingEnabled) {
                modificationCount++;
            }
        }
    }

	void setAll(T newSentinel) {
		sentinel = newSentinel;
		Data * newData = new Data();
		newData->mask = 0x00000000;
		newData->raw = new T[1];
		newData->raw[0] = sentinel;
		Data * oldData = data.exchange(newData);
		delete[] oldData->raw;
		delete oldData;

	}

	inline int x() const {
		return position.x;
	}
	inline int y() const {
		return position.y;
	}
	inline int z() const {
		return position.z;
	}
};

#define FOR_XYZ_TALEA for (int z = 0; z < TaleaDim; ++z) for (int y = 0; y < TaleaDim; ++y) for (int x = 0; x < TaleaDim; ++x)

#endif //TEST2_TALEA_H
