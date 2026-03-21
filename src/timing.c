#include <stdio.h>
#include <stdlib.h>

#include "sdlwrappers.h"
#include "timing.h"

/*
#define SECS_PER_MIN 60
#define MSECS_PER_SEC 1000
#define USECS_PER_MSEC 1000
Timespan getTimespan(Timeval before, Timeval after) {
    time_t      s_diff = labs(after.tv_sec - before.tv_sec);
    suseconds_t us_diff = abs(after.tv_usec - before.tv_usec);
    return (Timespan){
            .m = ((time_t)s_diff) / SECS_PER_MIN,
            .s = ((time_t)s_diff) % SECS_PER_MIN,
            .ms = ((time_t)us_diff) / USECS_PER_MSEC,
            .us = ((time_t)us_diff) % USECS_PER_MSEC,
    };
}

double TimespanSumMS(Timespan t) {
    double ms = t.ms;
    ms += t.s * 1000;
    ms += t.m * (1000 * 60);
    ms += t.us / 1000.0f;
    return ms;
}

void printTimespan(Timespan t) {
    // 03 m 27 s 123 ms 456 µs OR 03m27s (123 ms, 456 µs) OR 03:27:123:456
    LOG("%02ld m %02ld s %03ld ms %03ld µs\n", t.m, t.s, t.ms, t.us);
}
*/
