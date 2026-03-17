#include <math.h>

#include "sdlwrappers.h"
// T piece
const SDL_FColor purple[] = {
        rgb(80, 0, 120),
        rgb(144, 0, 216),
        rgb(160, 0, 240),
        rgb(221, 153, 255),
};

// S piece --__
const SDL_FColor green[] = {
        rgb(0, 120, 0),
        rgb(0, 216, 0),
        rgb(0, 240, 0),
        rgb(153, 255, 153),
};

// Z piece __--
const SDL_FColor red[] = {
        rgb(120, 0, 0),
        rgb(216, 0, 0),
        rgb(240, 0, 0),
        rgb(255, 153, 153),
};

// J piece _-_
const SDL_FColor blue[] = {
        rgb(0, 0, 120),
        rgb(0, 0, 216),
        rgb(0, 0, 240),
        rgb(153, 153, 255),
};

// O piece square
const SDL_FColor yellow[] = {
        rgb(120, 120, 0),
        rgb(216, 216, 0),
        rgb(240, 240, 0),
        rgb(255, 255, 153),
};

// L piece -_-
const SDL_FColor orange[] = {
        rgb(120, 80, 0),
        rgb(216, 144, 0),
        rgb(240, 160, 0),
        rgb(255, 221, 153),
};

// I piece
const SDL_FColor cyan[] = {
        rgb(0, 120, 120),
        rgb(0, 216, 216),
        rgb(0, 240, 240),
        rgb(153, 255, 255),
};

SDLContext ctx = {};
SDLContext init_ctx() {
    const size_t RB_CAP = 10000;
    return (SDLContext){
            .window = NULL,
            .renderer = NULL,
            .input = {},
            .perf = {.show = false,
                     .fps_rb = rb_create(sizeof(double), RB_CAP),
                     .ft_rb = rb_create(sizeof(double), RB_CAP)},
            .draw = {.clear_color = makergba(1.0, 1.0, 1.0, 1.0)},
            .clock_freq = NAN,
    };
}
