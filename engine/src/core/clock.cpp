#include "clock.h"

#include "platform/platform.h"

clock::clock(): start_time{0}, elapsed{0}{};

f64 clock::clock_update() {
    if (start_time != 0) {
        elapsed = platform_get_absolute_time() - start_time;
        return elapsed;
    }
    return 0;
}

void clock::clock_start() {
    start_time = platform_get_absolute_time();
    elapsed = 0;
}

void clock::clock_stop() {
    start_time = 0;
}