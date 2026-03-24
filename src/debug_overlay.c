#include <SDL3/SDL.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug_overlay.h"

#include "game/draw.h"
#include "logger.h"
#include "ringbuffer.h"
#include "sdlwrappers.h"
#include "sugar.h"
#include "types.h"

#define MAX_LINE_COUNT 256
#define MAX_LINE_LEN 4096
#define MAX_PUSH_LEN 4096
static char overlay_lines[MAX_LINE_COUNT][MAX_LINE_LEN];

typedef struct Overlay {
    vec2   s_topLeftPos;
    vec2   s_cursorPos;
    double font_height;
    double font_width;
    double vertical_spacing;

    const SDL_FColor font_color;
    size_t           line_count;
} Overlay;

const static float BASE_PX = 8.0;
// clang-format off
static Overlay overlay = {
    .s_topLeftPos = { 30, 30 },  
    .s_cursorPos = {30,30 },
    .font_height = 20.0,
    .font_width = 10.0,
    .vertical_spacing = BASE_PX/2.0,
    .font_color = rgb(255, 0, 255),
    .line_count = 0,
};
// clang-format on

void overlay_reset(void) {
    for (int i = 0; i < overlay.line_count; i++) {
        memset(overlay_lines[i], 0x0, MAX_LINE_LEN);
    }
    overlay.s_cursorPos.x = 0;
    overlay.s_cursorPos.y = 0;
    overlay.line_count = 0;
}

void overlay_push_line(const char* line, size_t n) {
    // push new linebuf
    memcpy(overlay_lines[overlay.line_count], line, n);
    overlay.line_count++;
}

// parse format string, taking into account newlines
void overlay_push_fstr(const char* fmt, ...) {
    char buf[MAX_PUSH_LEN];
    memset(buf, 0x0, MAX_PUSH_LEN);

    va_list args;
    va_start(args, fmt);
    size_t buf_len = vsnprintf(buf, MAX_PUSH_LEN, fmt, args);
    va_end(args);

    char*  current_line = NULL;
    size_t line_len = 0;
    for (int i = 0; i < buf_len; i++) {
        if (!current_line) {
            current_line = &buf[i];
        }
        if (buf[i] == '\n') {
            buf[i] = '\0';  // null terminate, replacing newline
            overlay_push_line(current_line, line_len);

            current_line =
                    NULL;  // next iteration it will be set to the following byte if it exists

            line_len = 0;
        } else {
            line_len++;
        }
    }
}

void drawDebugOverlay(bool bottom) {
    SDL_FColor saved_prev_color;
    getcolor(saved_prev_color);
    setcolor(overlay.font_color);
    double txt_scale = overlay.font_height / BASE_PX;
    SDL_SetRenderScale(ctx.renderer, txt_scale, txt_scale);
    vec2 s_cursorpos = overlay.s_cursorPos;
    vec2 s_scaledcpos = vec2_mul(overlay.s_cursorPos, txt_scale);

    if (bottom) {
        s_scaledcpos.y = (ctx.h / txt_scale) - (overlay.vertical_spacing * overlay.line_count);
        for (int i = overlay.line_count - 1; i >= 0; i--) {
            SDL_RenderDebugText(ctx.renderer, s_scaledcpos.x, s_scaledcpos.y, overlay_lines[i]);
            //            LOGLN("%lf,%lf", s_scaledcpos.x, s_scaledcpos.y);
            s_scaledcpos.y -= overlay.vertical_spacing * txt_scale;
        }
    } else {
        for (int i = 0; i < overlay.line_count; i++) {
            SDL_RenderDebugText(ctx.renderer, s_scaledcpos.x, s_scaledcpos.y, overlay_lines[i]);
            s_scaledcpos.y += overlay.vertical_spacing * txt_scale;
        }
    }

    SDL_SetRenderScale(ctx.renderer, 1.0, 1.0);
    setcolor(saved_prev_color);
}

void overlay_set_size(const double lf) {
    overlay.font_height = lf;
    overlay.font_width = lf / 2.0;
}
