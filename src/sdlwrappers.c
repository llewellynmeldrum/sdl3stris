#include <math.h>

#include "sdlwrappers.h"

const SDL_FColor red = (SDL_FColor){1.0, 0, 0, 1.0};
const SDL_FColor green = (SDL_FColor){0.0, 1.0, 0, 1.0};
const SDL_FColor blue = (SDL_FColor){0.0, 0, 1.0, 1.0};
const SDL_FColor grey = (SDL_FColor){0.3, 0.3, 0.3, 1.0};

// T piece
const SDL_FColor purple_0 = rgb(80, 0, 120);
const SDL_FColor purple_1 = rgb(144, 0, 216);
const SDL_FColor purple_2 = rgb(160, 0, 240);
const SDL_FColor purple_3 = rgb(221, 153, 255);

// S piece --__
const SDL_FColor green_0 = rgb(0, 120, 0);
const SDL_FColor green_1 = rgb(0, 216, 0);
const SDL_FColor green_2 = rgb(0, 240, 0);
const SDL_FColor green_3 = rgb(153, 255, 153);

// Z piece __--
const SDL_FColor red_0 = rgb(120, 0, 0);
const SDL_FColor red_1 = rgb(216, 0, 0);
const SDL_FColor red_2 = rgb(240, 0, 0);
const SDL_FColor red_3 = rgb(255, 153, 153);

// J piece _-_
const SDL_FColor blue_0 = rgb(0, 0, 120);
const SDL_FColor blue_1 = rgb(0, 0, 216);
const SDL_FColor blue_2 = rgb(0, 0, 240);
const SDL_FColor blue_3 = rgb(153, 153, 255);

// O piece square
const SDL_FColor yellow_0 = rgb(120, 120, 0);
const SDL_FColor yellow_1 = rgb(216, 216, 0);
const SDL_FColor yellow_2 = rgb(240, 240, 0);
const SDL_FColor yellow_3 = rgb(255, 255, 153);

// L piece -_-
const SDL_FColor orange_0 = rgb(120, 80, 0);
const SDL_FColor orange_1 = rgb(216, 144, 0);
const SDL_FColor orange_2 = rgb(240, 160, 0);
const SDL_FColor orange_3 = rgb(255, 221, 153);

// I piece
const SDL_FColor cyan_0 = rgb(0, 120, 120);
const SDL_FColor cyan_1 = rgb(0, 216, 216);
const SDL_FColor cyan_2 = rgb(0, 240, 240);
const SDL_FColor cyan_3 = rgb(153, 255, 255);

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
