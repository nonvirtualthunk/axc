//
// Created by Sam Bock on 9/23/16.
//

#ifndef TEST2_ACCUMULATINGSTATS_H
#define TEST2_ACCUMULATINGSTATS_H

template<typename T>
class AccumMin {
	T min;

public:
	AccumMin(T min) : min(min) {}

	void apply(const T& next) {
		if (next < min) {
			min = next;
		}
	}
};

template<typename T>
class AccumMax {
	T max;

public:
	AccumMax(T max) : max(max) {}

	void apply(const T& next) {
		if (next < max) {
			max = next;
		}
	}
};

template<typename T>
class AccumRange {
	AccumMin<T> min;
	AccumMax<T> max;

public:
	AccumRange(T minV, T maxV) : min(minV), max(maxV) {}

	void apply(const T& next) {
		min.apply(next);
		max.apply(next);
	}
};

template<typename Vec>
struct AccumVecRange3 {
	Vec min;
	Vec max;

	AccumVecRange3(const Vec& min, const Vec& max) : min(min), max(max) {}

	void apply(const Vec& v) {
		min.x = std::min(v.x,min.x);
		min.y = std::min(v.y,min.y);
		min.z = std::min(v.z,min.z);

		max.x = std::max(v.x,max.x);
		max.y = std::max(v.y,max.y);
		max.z = std::max(v.z,max.z);
	}
};

#endif //TEST2_ACCUMULATINGSTATS_H
