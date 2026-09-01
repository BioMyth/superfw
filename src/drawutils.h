#pragma once

#include <string.h>

#include "common.h"
#include "gbahw.h"
#include "util.h"
#include "res/icons.h"

#include "fonts/font_render.h"


#define ANIM_INITIAL_WAIT     128    // Intial wait (in anim cycles)
#define THREEDOTS_WIDTH      9

#define FG_COLOR         16
#define BG_COLOR         17
#define FT_COLOR         18
#define HI_COLOR         19
#define BL_COLOR         20

#define ICON_PAL   128

#define IGM_PAL_FG      240
#define IGM_PAL_BG      241
#define IGM_PAL_HI      242
#define IGM_PAL_SH      243
#define IGM_PAL_BL      244
#define SEL_COLOR       255

#define MAX_OBJS 64

typedef struct {
  uint16_t y, x;
  unsigned tn;
} oamobj_t;

struct frameState {
  oamobj_t objs[MAX_OBJS];
  unsigned objnum;
  unsigned framen;
} fstate = {
  .objs = {},
  .objnum = 0,
  .framen = 0
};

static inline bool push_sprite(oamobj_t sprite) {
  if (fstate.objnum >= MAX_OBJS)
    return false;
  fstate.objs[fstate.objnum++] = sprite;
  return true;
}

static inline void render_icon(unsigned x, unsigned y, unsigned iconn) { 
  push_sprite((oamobj_t){
    // Use 256 entries palette
    .y = y | 0x2000,
    // Size 16x16 
    .x = x | 0x4000,
    // OBJ numbers start at 512 for Mode 4
    .tn = 8 * iconn + 512
  });
}

static inline void render_icon_trans(unsigned x, unsigned y, unsigned iconn) { 
  push_sprite((oamobj_t){
    // Use 256 entries palette
    .y = y | 0x2400,
    // Size 16x16 
    .x = x | 0x4000,
    // OBJ numbers start at 512 for Mode 4
    .tn = 8 * iconn + 512
  });
}


// Split into two functions since one can be unrolled
static inline void render_bar_fs(volatile uint8_t *frame, unsigned y) {
  // TODO: Update this with direct memset like done for the top bar instead of sprite based
  dma_memset16(&frame[SCREEN_WIDTH * y], dup8(FG_COLOR), SCREEN_WIDTH*16/2);
  // SCREN_WIDTH / SPRITE_SIZE = 240 / 16 = 15
  // #pragma GCC unroll 15
  // for (unsigned i = 0; i < 15; i ++)
  //   render_icon_trans(i * 16, y, 63);
}

static inline void render_bar(unsigned startx, unsigned endx, unsigned y) {
  // TODO: Update this with direct memset like done for the top bar instead of sprite based
  //dma_memset16(&frame[SCREEN_WIDTH * y] + startx, dup8(FG_COLOR), SCREEN_WIDTH*16/2);
  // SCREN_WIDTH / SPRITE_SIZE = 240 / 16 = 15
  for (unsigned i = 0; i < ((startx - endx) / 16); i ++)
    render_icon_trans(startx + (i * 16), y, 63);
}


// Draws text adding some support for overflow.
#define THREEDOTS_WIDTH  9
static void draw_text_ovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x];
  unsigned twidth = font_width(t);
  if (twidth <= maxw)
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, FT_COLOR);
  else {
    char tmpbuf[256];
    unsigned numchars = font_width_cap(t, maxw - THREEDOTS_WIDTH);
    memcpy(tmpbuf, t, numchars);
    memcpy(&tmpbuf[numchars], "...", 4);
    draw_text_idx8_bus16(tmpbuf, basept, SCREEN_WIDTH, FT_COLOR);
  }
}

static void draw_text_leftovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x];
  unsigned numchars = font_width_lcap(t, maxw - THREEDOTS_WIDTH);
  if (numchars) {
    draw_text_idx8_bus16("...", basept, SCREEN_WIDTH, FT_COLOR);
    draw_text_idx8_bus16(&t[numchars], basept + THREEDOTS_WIDTH, SCREEN_WIDTH, FT_COLOR);
  } else {
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, FT_COLOR);
  }
}

static void draw_text_ovf_rotate(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw, unsigned *franim) {
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x];
  unsigned twidth = font_width(t);
  if (twidth <= maxw)
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, FT_COLOR);
  else {
    unsigned anim = *franim > ANIM_INITIAL_WAIT ? (*franim - ANIM_INITIAL_WAIT) >> 4 : 0;

    // Wrap around once the text end reaches the mid point aprox.
    char tmpbuf[540];
    strcpy(tmpbuf, t);
    strcat(tmpbuf, "      ");
    unsigned pixw = font_width(tmpbuf);
    if (anim > pixw)
      *franim = ANIM_INITIAL_WAIT + ((anim - pixw) << 4);
    strcat(tmpbuf, t);

    draw_text_idx8_bus16_range(tmpbuf, basept, anim, maxw, SCREEN_WIDTH, FT_COLOR);
  }
}

static void draw_box_outline(volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom, uint8_t color) {
  dma_memset16(&frame[SCREEN_WIDTH * top + left], dup8(color), (right - left) / 2);
  dma_memset16(&frame[SCREEN_WIDTH * (top + 1) + left], dup8(color), (right - left) / 2);
  dma_memset16(&frame[SCREEN_WIDTH * (bottom - 1) + left], dup8(color), (right - left) / 2);
  dma_memset16(&frame[SCREEN_WIDTH * (bottom - 2) + left], dup8(color), (right - left) / 2);
  while (top < bottom) {
    *((uint16_t*)&frame[SCREEN_WIDTH * top + left]) = dup8(color);
    *((uint16_t*)&frame[SCREEN_WIDTH * top + right - 2]) = dup8(color);
    top++;
  }
}

static void draw_box_full(
  volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom,
  uint8_t outlinecolor, uint8_t bgcolor
) {
  draw_box_outline(frame, left, right, top, bottom, outlinecolor);
  for (unsigned i = top + 2; i < bottom - 2; i++)
    dma_memset16(&frame[SCREEN_WIDTH * i + left + 2], dup8(bgcolor), (right - left - 4) / 2);
}

static void draw_button_box(
  volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom, bool selected
) {
  if (selected)
    draw_box_full(frame, left, right, top, bottom, FG_COLOR, HI_COLOR);
  else
    draw_box_outline(frame, left, right, top, bottom, FG_COLOR);
}


static void draw_rightj_text(const char *t, volatile uint8_t *frame, unsigned x, unsigned y) {
  unsigned twidth = font_width(t);
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - twidth];
  draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, FT_COLOR);
}

static void draw_central_text(const char *t, volatile uint8_t *frame, unsigned x, unsigned y) {
  unsigned twidth = font_width(t);
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - twidth / 2];
  draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, FT_COLOR);
}

static void draw_central_text_ovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  unsigned twidth = font_width(t);
  if (twidth <= maxw) {
    uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - twidth / 2];
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, FT_COLOR);
  } else {
    char tmpbuf[256];
    unsigned numchars = font_width_cap(t, maxw - THREEDOTS_WIDTH);
    memcpy(tmpbuf, t, numchars);
    memcpy(&tmpbuf[numchars], "...", 4);
    uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - maxw / 2];
    draw_text_idx8_bus16(tmpbuf, basept, SCREEN_WIDTH, FT_COLOR);
  }
}

static void draw_central_text_wrapped(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  while (*t) {
    char tmp[128];
    unsigned outw;
    unsigned linechars = font_width_cap_space(t, maxw, &outw);
    unsigned charcnt = linechars ?: utf8_strlen(t);
    uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - outw / 2];

    memcpy(tmp, t, charcnt);
    tmp[charcnt] = 0;
    draw_text_idx8_bus16(tmp, basept, SCREEN_WIDTH, FT_COLOR);

    t += charcnt;      // Advance text
    y += 16;           // Move down in the buffer
  }
}

static void draw_progress_bar(unsigned done, unsigned total) {
  // Draws and flips the buffer, do not care about vsync here
  volatile uint8_t *frame = &MEM_VRAM_U8[0xA000*fstate.framen];

  // Render the full background to a solid color
  dma_memset16(&frame[0], dup8(BG_COLOR), SCREEN_WIDTH*SCREEN_HEIGHT/2);

  // Render a simple progress bar
  unsigned prog = done * 200 / total;
  for (unsigned i = 76; i < 84; i++)
    dma_memset16(&frame[SCREEN_WIDTH * i + 20], dup8(FG_COLOR), prog/2);

  dma_memset16(MEM_OAM, 0, 256);  // Clear icons

  REG_DISPCNT = (REG_DISPCNT & ~0x10) | (fstate.framen << 4);
  fstate.framen ^= 1;
}

// Guess the file type based on the file name.
static unsigned guessicon(const char *path) {
  unsigned l = strlen(path);
  if (l < 4)
    return ICON_BINFILE;

  if (!strcasecmp(&path[l-4], ".gba"))
    return ICON_GBACART;
  else if (!strcasecmp(&path[l-3], ".gb"))
    return ICON_GBCART;
  else if (!strcasecmp(&path[l-4], ".gbc"))
    return ICON_GBCCART;
  else if (!strcasecmp(&path[l-4], ".nes"))
    return ICON_NESCART;
  else if (!strcasecmp(&path[l-4], ".sms"))
    return ICON_SMSCART;
  else if (!strcasecmp(&path[l-3], ".fw"))
    return ICON_UPDFILE;

  return ICON_BINFILE;
}

void render_browser_row(volatile uint8_t *frame, char *fn, uint16_t attr, unsigned row, bool selected, uint32_t sz, unsigned int *franim) {
  unsigned int szstrwidth = 0;
  if (sz && !(attr & AM_DIR)){
    // Animate the row entries if they are too long!
    char szstr[16];
    human_size(szstr, sizeof(szstr), sz);
    draw_rightj_text(szstr, frame, SCREEN_WIDTH - 2, (1 + row) * 16);
    szstrwidth = font_width(szstr) + 2;
  }
  unsigned int icon;
  if (attr & AM_HID || fn[0] == '.')
    icon = (attr & AM_DIR ? ICON_HFOLDER : ICON_HFILE);
  else
    icon = (attr & AM_DIR ? ICON_FOLDER : guessicon(fn));

  render_icon(2, (row+1)*16, icon);
  // Animate the row entries if they are too long!
  if (selected)
    draw_text_ovf_rotate(fn, frame, 20, (1 + row) * 16,
                          SCREEN_WIDTH - 24 - szstrwidth, franim);
  else
    draw_text_ovf(fn, frame, 20, (1 + row) * 16, 
      SCREEN_WIDTH - 24 - szstrwidth);
}


void menu_flip() {
  /* Copy icons directly instead of iterating */
  dma_memcpy16(&MEM_OAM[0], fstate.objs, fstate.objnum * 4);
  dma_memset16(&MEM_OAM[fstate.objnum*4], 0, 256 - fstate.objnum*2);  // Clear unused objects
  REG_DISPCNT = (REG_DISPCNT & ~0x10) | (fstate.framen << 4);
  fstate.framen ^= 1;
}