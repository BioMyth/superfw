#pragma once
#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include <string.h>

#include "common.h"
#include "gbahw.h"
#include "util.h"
#include "res/icons.h"
#include "fatfs/ff.h"
#include "fonts/font_render.h"


#define ANIM_INITIAL_WAIT     128    // Intial wait (in anim cycles)
#define THREEDOTS_WIDTH      9

#define ICON_PAL   128

#define MAX_OBJS 64

/*
* Standard Menu Colors
*/
enum MenuColor {
  MENU_COLOR_FG = 16,
  MENU_COLOR_BG,
  MENU_COLOR_FT,
  MENU_COLOR_HI,
  MENU_COLOR_SEL = 255
};
/*
* In Game Menu Colors
*/
enum IGMColor {
  IGM_COLOR_FG = 240,
  IGM_COLOR_BG,
  IGM_COLOR_HI,
  IGM_COLOR_SH,
  IGM_COLOR_BL,
  IGM_COLOR_SEL = 255
};

struct oamobj_t{
  uint16_t y, x;
  unsigned tn;
};

struct frame_state {
  struct oamobj_t objs[MAX_OBJS];
  unsigned objnum;
  unsigned framen;
};

void reset_sprites();

unsigned get_frame_num();

bool push_sprite(struct oamobj_t sprite);

void render_icon(unsigned x, unsigned y, unsigned iconn);
void render_icon_trans(unsigned x, unsigned y, unsigned iconn);

void render_bar(volatile uint8_t *frame, uint8_t color, unsigned x, unsigned y, uint8_t width, uint8_t height);

void draw_text_ovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw);

void draw_text_leftovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw);

void draw_text_ovf_rotate(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw, unsigned *franim);

void draw_box_outline(volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom, uint8_t color);

void draw_box_full(
  volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom,
  uint8_t outlinecolor, uint8_t bgcolor
);

void draw_button_box(
  volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom, bool selected
);


void draw_rightj_text(const char *t, volatile uint8_t *frame, unsigned x, unsigned y);

void draw_central_text(const char *t, volatile uint8_t *frame, unsigned x, unsigned y);

void draw_central_text_ovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw);

void draw_central_text_wrapped(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw);

void draw_progress_bar(unsigned done, unsigned total);

// Guess the file type based on the file name.
unsigned guessicon(const char *path);

void render_browser_row(volatile uint8_t *frame, char *fn, uint16_t attr, unsigned row, bool selected, uint32_t sz, unsigned int *franim);


void menu_flip();
#endif