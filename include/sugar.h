#pragma once
#include "types.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define LEN(expr) sizeof(expr)  // been using too much python

#define bound(val, lo, hi) (fmin(hi, (fmax(lo, val))))
