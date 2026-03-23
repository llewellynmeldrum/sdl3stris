#pragma once
#include "types.h"
#include <stdbool.h>
// NOTE: implemented by:
// src/debug_overlay.c
#define OVERLAY_PRINTLN(fmt, ...) overlay_push_fstr(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#define OVERLAY_RESET() overlay_reset()

void overlay_push_fstr(const char* fmt, ...);
void overlay_set_size(const double lf);
void drawDebugOverlay(bool bottom);
void overlay_reset(void);
