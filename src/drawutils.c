#include "drawutils.h"

struct frame_state fstate = {
  .objs = {},
  .objnum = 0,
  .framen = 0
};

inline void reset_sprites() {
  fstate.objnum = 0;
}

inline unsigned get_frame_num(){
  return fstate.framen;
}

inline bool push_sprite(struct oamobj_t sprite) {
  if (fstate.objnum >= MAX_OBJS)
    return false;
  fstate.objs[fstate.objnum++] = sprite;
  return true;
}

inline void render_icon(unsigned x, unsigned y, unsigned iconn) { 
  push_sprite((struct oamobj_t){
    // Use 256 entries palette
    .y = y | 0x2000,
    // Size 16x16 
    .x = x | 0x4000,
    // OBJ numbers start at 512 for Mode 4
    .tn = 8 * iconn + 512
  });
}

inline void render_icon_trans(unsigned x, unsigned y, unsigned iconn) { 
  push_sprite((struct oamobj_t){
    // Use 256 entries palette
    .y = y | 0x2400,
    // Size 16x16 
    .x = x | 0x4000,
    // OBJ numbers start at 512 for Mode 4
    .tn = 8 * iconn + 512
  });
}


// Split into two functions since one can be unrolled
inline void render_bar(volatile uint8_t *frame, uint8_t color, unsigned x, unsigned y, uint8_t width, uint8_t height) {
  // Prevent over render scenarios
  if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
    return;
  
  // Clamp range to the frame buffer
  width = MIN(width, SCREEN_WIDTH - x);
  height = MIN(height, SCREEN_HEIGHT - y);

  if (width == SCREEN_WIDTH && x == 0)
    // If bar is full width, we can memset all pixel rows at once
    dma_memset16(&frame[SCREEN_WIDTH * y], dup8(color), SCREEN_WIDTH * height / 2);
  else {
    // If bar is not full width, we have to set each pixel row independently
    for (uint8_t offy = 0; offy < height; offy++) {
      dma_memset16(&frame[SCREEN_WIDTH * (offy + y) + x], dup8(color), width/2);
    }
    
  }
}


void draw_text_ovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x];
  unsigned twidth = font_width(t);
  if (twidth <= maxw)
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, MENU_COLOR_FT);
  else {
    char tmpbuf[256];
    unsigned numchars = font_width_cap(t, maxw - THREEDOTS_WIDTH);
    memcpy(tmpbuf, t, numchars);
    memcpy(&tmpbuf[numchars], "...", 4);
    draw_text_idx8_bus16(tmpbuf, basept, SCREEN_WIDTH, MENU_COLOR_FT);
  }
}

void draw_text_leftovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x];
  unsigned numchars = font_width_lcap(t, maxw - THREEDOTS_WIDTH);
  if (numchars) {
    draw_text_idx8_bus16("...", basept, SCREEN_WIDTH, MENU_COLOR_FT);
    draw_text_idx8_bus16(&t[numchars], basept + THREEDOTS_WIDTH, SCREEN_WIDTH, MENU_COLOR_FT);
  } else {
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, MENU_COLOR_FT);
  }
}

void draw_text_ovf_rotate(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw, unsigned *franim) {
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x];
  unsigned twidth = font_width(t);
  if (twidth <= maxw)
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, MENU_COLOR_FT);
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

    draw_text_idx8_bus16_range(tmpbuf, basept, anim, maxw, SCREEN_WIDTH, MENU_COLOR_FT);
  }
}

void draw_box_outline(volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom, uint8_t color) {
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

void draw_box_full(
  volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom,
  uint8_t outlinecolor, uint8_t bgcolor
) {
  draw_box_outline(frame, left, right, top, bottom, outlinecolor);
  for (unsigned i = top + 2; i < bottom - 2; i++)
    dma_memset16(&frame[SCREEN_WIDTH * i + left + 2], dup8(bgcolor), (right - left - 4) / 2);
}

void draw_button_box(
  volatile uint8_t *frame, unsigned left, unsigned right, unsigned top, unsigned bottom, bool selected
) {
  if (selected)
    draw_box_full(frame, left, right, top, bottom, MENU_COLOR_FG, MENU_COLOR_HI);
  else
    draw_box_outline(frame, left, right, top, bottom, MENU_COLOR_FG);
}


void draw_rightj_text(const char *t, volatile uint8_t *frame, unsigned x, unsigned y) {
  unsigned twidth = font_width(t);
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - twidth];
  draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, MENU_COLOR_FT);
}

void draw_central_text(const char *t, volatile uint8_t *frame, unsigned x, unsigned y) {
  unsigned twidth = font_width(t);
  uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - twidth / 2];
  draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, MENU_COLOR_FT);
}

void draw_central_text_ovf(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  unsigned twidth = font_width(t);
  if (twidth <= maxw) {
    uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - twidth / 2];
    draw_text_idx8_bus16(t, basept, SCREEN_WIDTH, MENU_COLOR_FT);
  } else {
    char tmpbuf[256];
    unsigned numchars = font_width_cap(t, maxw - THREEDOTS_WIDTH);
    memcpy(tmpbuf, t, numchars);
    memcpy(&tmpbuf[numchars], "...", 4);
    uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - maxw / 2];
    draw_text_idx8_bus16(tmpbuf, basept, SCREEN_WIDTH, MENU_COLOR_FT);
  }
}

void draw_central_text_wrapped(const char *t, volatile uint8_t *frame, unsigned x, unsigned y, unsigned maxw) {
  while (*t) {
    char tmp[128];
    unsigned outw;
    unsigned linechars = font_width_cap_space(t, maxw, &outw);
    unsigned charcnt = linechars ?: utf8_strlen(t);
    uint8_t *basept = (uint8_t*)&frame[y * SCREEN_WIDTH + x - outw / 2];

    memcpy(tmp, t, charcnt);
    tmp[charcnt] = 0;
    draw_text_idx8_bus16(tmp, basept, SCREEN_WIDTH, MENU_COLOR_FT);

    t += charcnt;      // Advance text
    y += 16;           // Move down in the buffer
  }
}

void draw_progress_bar(unsigned done, unsigned total) {
  // Draws and flips the buffer, do not care about vsync here
  volatile uint8_t *frame = &MEM_VRAM_U8[0xA000*fstate.framen];

  // Render the full background to a solid color
  dma_memset16(&frame[0], dup8(MENU_COLOR_BG), SCREEN_WIDTH*SCREEN_HEIGHT/2);

  // Render a simple progress bar
  unsigned prog = done * 200 / total;
  for (unsigned i = 76; i < 84; i++)
    dma_memset16(&frame[SCREEN_WIDTH * i + 20], dup8(MENU_COLOR_FG), prog/2);

  dma_memset16(MEM_OAM, 0, 256);  // Clear icons

  REG_DISPCNT = (REG_DISPCNT & ~0x10) | (fstate.framen << 4);
  fstate.framen ^= 1;
}

// Guess the file type based on the file name.
unsigned guessicon(const char *path) {
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
  // Divide by 4 since memcpy32 copies 4 bytes at a time
  dma_memcpy32(&MEM_OAM[0], fstate.objs, fstate.objnum * sizeof(struct oamobj_t)/4);
  dma_memset16(&MEM_OAM[fstate.objnum*4], 0, 256 - fstate.objnum*2);  // Clear unused objects
  REG_DISPCNT = (REG_DISPCNT & ~0x10) | (fstate.framen << 4);
  fstate.framen ^= 1;
}