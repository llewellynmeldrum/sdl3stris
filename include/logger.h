#pragma once

#define LOGERR(fmt, ...)                                                                           \
    fprintf(stderr, "[ERROR] in %s:%d->" fmt, __FILE__, __LINE__, ##__VA_ARGS__)
