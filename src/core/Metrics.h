//
// Created by nvt on 8/21/16.
//

#ifndef TEST2_METRICS_H
#define TEST2_METRICS_H

#include <chrono>
#include <Noto.h>

using namespace std::chrono;

class MetricsTimer {
protected:
    high_resolution_clock::time_point startTime;
    const char *name;

public:
    MetricsTimer(const char *name) :
            startTime(high_resolution_clock::now()),
            name(name) {

    }

    virtual ~MetricsTimer() {

    }

    void printElapsed() {
        auto delta = high_resolution_clock::now() - startTime;
        long long nanos = duration_cast<nanoseconds>(delta).count();
        if (nanos >= 1000000) {
            Noto::info("[{}] Elapsed: {} ms",name,nanos / 1000000);
        } else {
            Noto::info("[{}] Elapsed: {} nanos",name,nanos);
        }
    }
};


#define TIMER(name) \
MetricsTimer timer_##name(#name)


#define PRINT_TIMER(name) \
timer_##name.printElapsed()

#endif //TEST2_METRICS_H
