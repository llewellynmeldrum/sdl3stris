#pragma once
// clang-format off

#include "types.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define END_STYLE "\e[0m"
#define BOLD "\e[1m"
#define RED "\e[31m"
#define BOLD_RED "\e[1;31m"
#define GREEN "\e[32m"
#define BOLD_GREEN "\e[1;32m"
#define YELLOW "\e[34m"
#define BOLD_YELLOW "\e[1;34m"
// Given two heap allocated strings, insert `src` starting at `dst[pos]`.
// dst is reallocated to be the size of the two strings combined, i.e strlen(src)+strlen(dst)+1.
// (The null pointer of src is not copied.

#define OSTREAM stdout

#define LIST_TYPES                                                                                 \
    X(int, "%d")                                                                                   \
    X(uint64_t, "%llu")                                                                            \
    X(double, "%lf")

#define X(T, fmt)                                                                                  \
    static inline void _str_##T(const char* name, T val, const char* filename, int line) {         \
        fprintf(stderr, "\e[1m[%s:%d]:\e[0m %s= " fmt " \n", filename, line, name, val);           \
    }
LIST_TYPES

#undef X

#define FMT_STR(x)                                                                                 \
    _Generic(x, int: _str_int, uint64_t: _str_uint64_t, double: _str_double, default: _str_int)(   \
            #x, x, __FILE__, __LINE__)

// prints #expr=expr, eg v = 123. Each type must have a _str_<T> function defined
#define LOGEXPR(expr) FMT_STR(expr)

#define LOG_INFO() fprintf(OSTREAM, "[%s:%d]:", __FILE__, __LINE__)
#define LOG_INFO_FN() fprintf(OSTREAM, "[%s:%d:%s()]: ", __FILE__, __LINE__, __FUNCTION__)

#define LOG(fmt, ...) fprintf(OSTREAM, fmt, ##__VA_ARGS__)

#define LOGLN(fmt, ...)                                                                            \
    do {                                                                                           \
        LOG(fmt, ##__VA_ARGS__);                                                      \
        LOG("\n");                                                                                 \
    } while (0)

#define LOGFN(fmt, ...)                                                                            \
    do {                                                                                           \
        LOG_INFO_FN();                                                                              \
        LOGLN(fmt, ##__VA_ARGS__);                                                     \
    } while (0)

#ifdef CLEANUP_FN
#define LOGEXIT(exit_code)                                                                         \
    do {                                                                                           \
        if (exit_code == 0)                                                                        \
            LOG(BOLD_GREEN);                                                           \
        else                                                                                       \
            LOG(BOLD_RED);                                                             \
        LOGLN("Exiting with code %d...", exit_code);                                               \
        CLEANUP_FN(CLEANUP_FN_ARGS);                                                               \
        LOG(END_STYLE);                                                                             \
        exit(exit_code);\
    } while (0)
#else
#define LOGEXIT(exit_code)                                                                         \
    do {                                                                                           \
        if (exit_code == 0)                                                                        \
            LOG(GREEN);                                                           \
        else                                                                                       \
            LOG(RED);                                                             \
        LOGLN("Exiting... (%d)", exit_code);                                               \
        LOG(END_STYLE);                                                                             \
        exit(exit_code);\
    } while (0)
#endif

// exits the program after making a bold, red log
#define LOGFATAL(fmt, ...)                                                                         \
    do {                                                                                           \
        LOG(BOLD_RED);                                                                 \
        LOG("[FATAL ERROR] in ->");                                                                \
        LOG_INFO_FN();                                                                             \
        LOG(END_STYLE);                                                                            \
        LOGLN(RED);\
        LOGLN(fmt, ##__VA_ARGS__);                                                    \
        LOG(END_STYLE);                                                                            \
        LOGEXIT(EXIT_FAILURE);                                                                     \
    } while (0)

#define LOGERR(fmt, ...)                                                                           \
    do {                                                                                           \
        LOG(RED);                                                                      \
        LOG("[ERROR] in ->");                                                                      \
        LOG_INFO_FN();                                                                              \
        LOG(END_STYLE);                                                                            \
        LOGLN(fmt, ##__VA_ARGS__);                                                     \
    } while (0)
#define LOGNOTICE(fmt, ...)                                                                           \
    do {                                                                                           \
        LOG(YELLOW);                                                                      \
        LOG("[NOTICE] in ->");                                                                      \
        LOG_INFO_FN();                                                                              \
        LOG(END_STYLE);                                                                            \
        LOGLN(fmt, ##__VA_ARGS__);                                                     \
    } while (0)
