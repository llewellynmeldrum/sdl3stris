#pragma once
#include "logger.h"
#include "sdlwrappers.h"
#include <sys/time.h>
// #define DEBUG

static inline double get_current_ms(double freq) {
    double counter = SDL_GetPerformanceCounter();
    double res = (counter / freq) * 1000.0;

#ifdef DEBUG
    LOGLN("counter:%lf, res:%lf", counter, res);
#endif
    return res;
}

static inline double ms_since_start(SDLContext* ctx) {
    return get_current_ms(ctx->clock_freq) - ctx->ms_at_start;
}
#undef DEBUG
/*
typedef struct timeval Timeval;

inline void RecordTime(Timeval t) {
    gettimeofday(&t, NULL);
}

inline Timeval GetTime() {
    Timeval t;
    gettimeofday(&t, NULL);
    return t;
}

#define BenchmarkFunction(span, callable, ...)                                                     \
    do {                                                                                           \
        Timeval before, after;                                                                     \
        RecordTime(before);                                                                        \
        callable(__VA_ARGS__);                                                                     \
        RecordTime(after);                                                                         \
        span = GetTimespan(before, after);                                                         \
    } while (0)

typedef struct timespan {
    time_t m;
    time_t s;
    time_t ms;
    time_t us;
} Timespan;

Timespan getTimespan(Timeval before, Timeval after);
void     printTimespan(Timespan t);
*/
