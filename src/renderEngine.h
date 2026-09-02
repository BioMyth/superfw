#pragma once

#include "gbahw.h"
#include "common.h"
#include "messages.h"
#include "drawutils.h"
#include "nanoprintf.h"
#include "settings.h"

#define TABS_HEIGHT 22

#define ROW_HEIGHT 20

#define ROW_COUNT 6
//(SCREEN_HEIGHT - TABS_HEIGHT)/ ROW_HEIGHT
//6

// First entries reserved for the logo palette.
#define FG_COLOR         16
#define BG_COLOR         17
#define FT_COLOR         18
#define HI_COLOR         19
#define IGM_PAL_FG      240
#define IGM_PAL_BG      241
#define IGM_PAL_HI      242
#define IGM_PAL_SH      243
#define IGM_PAL_BL      244
#define SEL_COLOR       255

#define TMP_BUF_SIZE 128
#define TOP_BAR_HEIGHT 16
#define SUBMENU_BORDER 2

#define MENU_MARGIN 8

typedef enum menuOptionType_e {
  Header,
  RTC,
  IntScroll,
  TxtScroll,
  ArrScroll,
  Bool,
  InvBool,
  HotKey,
  Callback,
  Button,
  SelectableRow,
  SubMenuHeader
} menuOptionType_e;

typedef enum menuType_e {
  Standard,
  Submenu,
  Popup
} menuType_e;


typedef void (*menuoptioncallback_t)(char *tmpbuf, uint16_t bufsize);
typedef void (*menubottcallback_t)(char *tmpbuf, uint8_t selectedOption);

typedef struct {
    /* Translation Id */
  uint32_t name;
  menuOptionType_e type;
  /* Translation Id */
  uint32_t baseValue;
  uint8_t *selectedValue;
  // bool animate;
  menuoptioncallback_t callback;
} menuoption_t;

typedef struct {
  menuType_e type;
  int *selector;
  uint16_t optionCount;
  const menuoption_t *options;
  menubottcallback_t bottCallback;
} menu_t;

static void render_scrollable_text (char *tmpbuf, volatile uint8_t *frame, uint8_t x, uint8_t y, uint8_t maxw, unsigned *anim_state) {
    unsigned twidth = font_width(tmpbuf);
    // Render rotating text if exceeding max width & max width isn't 0
    if (twidth > maxw && maxw)
      draw_text_ovf_rotate(tmpbuf, frame, x, y, maxw, anim_state);
    else
      // Center the x value for the text on the reserved text space
      draw_central_text_ovf(tmpbuf, frame, x + maxw / 2, y, maxw);
}

static inline void render_setting_row(volatile uint8_t *frame, const char *title, const char *value, uint8_t x, uint8_t y) {
  draw_text_ovf(title, frame, x, y, SCREEN_WIDTH - 2*x);
  draw_central_text(value, frame, 170, y);
}


static void render_menu_option(volatile uint8_t *frame, unsigned *anim_state, const menuoption_t *option, bool selected, uint8_t x, uint8_t y) {
  char tmpbuf[TMP_BUF_SIZE];
  switch (option->type)
  {
  case Header:
    draw_central_text(msgs[lang_id][option->name], frame, SCREEN_WIDTH/2, y);
    break;
  case TxtScroll:
    const char *msg = msgs[lang_id][option->baseValue + (option->selectedValue == NULL ? 0 : *option->selectedValue)];
    if (msg[0] != '<'){
      npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %s >", 
        msg);
      render_setting_row(frame, msgs[lang_id][option->name], tmpbuf, x, y);
    }
    else  {
      render_setting_row(frame, msgs[lang_id][option->name], msg, x, y);
    }
    break;
  case IntScroll:
    npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %i >", *((uint8_t*) option->selectedValue));
    render_setting_row(frame, msgs[lang_id][option->name], tmpbuf, x, y);
    break;
  case Bool:
    render_setting_row(frame, msgs[lang_id][option->name], 
      msgs[lang_id][(option->baseValue ? option->baseValue : MSG_KNOB_DISABLED)+ *((bool *) option->selectedValue)], x, y);
    break;
  case InvBool:
    render_setting_row(frame, msgs[lang_id][option->name], 
      msgs[lang_id][(option->baseValue ? option->baseValue : MSG_KNOB_DISABLED) + 1 - *((bool *) option->selectedValue)],
      x, y);
    break;
  case Callback:
    option->callback(tmpbuf, TMP_BUF_SIZE);
    render_setting_row(frame, msgs[lang_id][option->name], tmpbuf, x, y);
    break;
  case Button:
    // Buttons handle their own highlighting in the button box rendering
    draw_button_box(frame, 20, SCREEN_WIDTH - 20,
      y, y + ROW_HEIGHT,
      selected);
    draw_central_text(msgs[lang_id][option->name], frame, SCREEN_WIDTH / 2, y + 2);
    break;
  case SelectableRow:
    draw_text_ovf(msgs[lang_id][option->name], frame, 22, y, 144);
    if (selected) {
      draw_central_text("▸", frame, MENU_MARGIN + ((*anim_state & 0xFF) >> 6), y);
    }
    break;
  default:
    break;
  }
}

void render_menu(volatile uint8_t *frame, unsigned *anim_state, const menu_t *menu) {
  char tmpbuf[TMP_BUF_SIZE];
  uint8_t numrows = (menu->bottCallback == NULL ? ROW_COUNT : ROW_COUNT - 1);
  
  unsigned selector = *menu->selector;

  // Odd number round up, e.g. 5 -> we want 3
  uint8_t halfnumrows = numrows / 2;
  
  bool scroll = menu->optionCount > numrows;
  bool lastpage = (menu->optionCount - selector - 1) <= halfnumrows;
  bool firstpage = selector <= halfnumrows;

  
  uint8_t offy = (scroll ? TABS_HEIGHT + 7 : TABS_HEIGHT);
  uint8_t offx = (menu->type == Submenu ? 12 : MENU_MARGIN);

  if (menu->type == Submenu)
    draw_box_outline(frame, 
      SUBMENU_BORDER, SCREEN_WIDTH - SUBMENU_BORDER, 
      TOP_BAR_HEIGHT + SUBMENU_BORDER, SCREEN_HEIGHT - SUBMENU_BORDER, 
      FG_COLOR);


  if (scroll && !firstpage)
    draw_central_text("⯅", frame, SCREEN_WIDTH / 2, 15);
  if (scroll && !lastpage)
    draw_central_text("⯆", frame, SCREEN_WIDTH / 2, ROW_HEIGHT * (numrows + 1));//125);

  uint8_t baseopt;
  // If we are in the first half of the first page or there aren't enough rows to scroll
  if (firstpage || !scroll)
    baseopt = 0;
  // If we are in the second half of the last page
  else if (lastpage) 
    baseopt = menu->optionCount - numrows;
  else 
    baseopt = selector - halfnumrows;
  
  // Render the highlight bar if not on a button
  // Render before text rendering occurrs so it is layered
  if (menu->options[*menu->selector].type != Button)
    render_bar(frame, HI_COLOR, 0, offy + (selector - baseopt) * ROW_HEIGHT, SCREEN_WIDTH, 16);
 
  for (uint8_t row = 0; row < MIN(numrows, menu->optionCount); row++)
  {
    uint16_t curropt = baseopt + row;
    if (curropt >= menu->optionCount)
      break;
    render_menu_option(
      frame,
      anim_state, 
      &menu->options[baseopt + row], 
      (*menu->selector) == baseopt + row, 
      offx,
      offy + row * ROW_HEIGHT
    );
  }
  // Render the bottom bar for Helptext or additional context
  if (menu->bottCallback != NULL){
    menu->bottCallback(tmpbuf, selector);
    if (menu->type == Standard){
      render_bar(frame, FG_COLOR, 0, SCREEN_HEIGHT - ROW_HEIGHT, SCREEN_WIDTH, ROW_HEIGHT);
      draw_text_ovf_rotate(tmpbuf, frame, offx, SCREEN_HEIGHT - 18,
                        SCREEN_WIDTH - offx * 2, anim_state);
    } else{
      render_bar(frame, FG_COLOR, offx, SCREEN_HEIGHT - ROW_HEIGHT - SUBMENU_BORDER, SCREEN_WIDTH - offx * 2, 16);
      render_scrollable_text(tmpbuf, frame, offx, SCREEN_HEIGHT - ROW_HEIGHT - SUBMENU_BORDER, SCREEN_WIDTH - offx * 2, anim_state);
    }
  }
}
