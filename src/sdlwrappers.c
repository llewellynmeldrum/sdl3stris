#include <math.h>

#include "logger.h"
#include "sdlwrappers.h"
#include "types.h"

// clang-format off
SDLContext ctx;
#define DEFAULT_PRESS_DELAY_MS 80
SDLContext init_ctx() {
    const size_t RB_CAP = 10000;

    SDLContext default_ctx = {
        .input = {
            .PRESS_DELAY_MS = DEFAULT_PRESS_DELAY_MS,
            .key_repeat_delay_ms_remaining = DEFAULT_PRESS_DELAY_MS,
        },

        .perf = {
            .show_perf_in_debug = true,
            .fps_rb = rb_create(sizeof(double), RB_CAP),
            .ft_rb = rb_create(sizeof(double), RB_CAP),
            .ms_lastframe = 0.0,
            .ms_thisframe = 0.0,
        },

        .draw = {
            .clear_color = rgba(0.0, 0.0, 0.0, 255.0)
        },
        .window = NULL,
        .renderer = NULL,
        .clock_freq = NAN,
        .w = DEF_WIDTH,
        .h = DEF_HEIGHT,
        .cols = DEF_COLS,
        .rows = DEF_ROWS

    };
    LOGLN("thisframe: %lf",default_ctx.perf.ms_thisframe);
    LOGLN("lastframe: %lf",default_ctx.perf.ms_lastframe);
    return default_ctx;
}
