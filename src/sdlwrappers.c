#include <assert.h>
#include <math.h>

#include "logger.h"
#include "sdlwrappers.h"
#include "timing.h"
#include "types.h"

// clang-format off
SDLContext ctx;


#define DEFAULT_PRESS_DELAY_MS 80
SDLContext init_ctx(SDL_Window* win, SDL_Renderer* rend, size_t w, size_t h) {
    if (!win){
        LOGERR("SDL window is null. SDLContext cannot be initialized without a valid window.");
    }
    if (!rend){
        LOGERR("SDL renderer is null. SDLContext cannot be initialized without a valid renderer.");
    }
    assert(w> 0 && h>0 || "Invalid width/height passed to " || __FUNCTION__);


    const size_t RB_CAPACITY = 10000;
    SDLContext default_ctx = {
        .input = {
            .PRESS_DELAY_MS = DEFAULT_PRESS_DELAY_MS,
            .key_repeat_delay_ms_remaining = DEFAULT_PRESS_DELAY_MS,
        },

        .perf = {
            .show_perf_in_debug = true,
            .fps_rb = rb_create(sizeof(double), RB_CAPACITY),
            .ft_rb = rb_create(sizeof(double), RB_CAPACITY),
            .ms_lastframe = 0.0,
            .ms_thisframe = 0.0,
        },

        .draw = {
            .clear_color = rgba(0.0, 0.0, 0.0, 255.0)
        },
        .window = win,
        .renderer = rend,
        .w = w,
        .h = h,
        .cols = DEF_COLS,
        .rows = DEF_ROWS,
        

    };
    default_ctx.clock_freq = SDL_GetPerformanceFrequency(),
    


    default_ctx.ms_at_start = get_current_ms(default_ctx.clock_freq);
    return default_ctx;
}
