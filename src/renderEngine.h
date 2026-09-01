#pragma once

#include "gbahw.h"
#include "common.h"
#include "messages.h"
#include "drawutils.h"
#include "nanoprintf.h"
#include "settings.h"

#define ROW_COUNT 6

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
  Button
} menuOptionType;

typedef void (*menuoptioncallback_t)(char *tmpbuf, uint16_t bufsize);
typedef void (*menuhelpcallback_t)(volatile uint8_t *frame);

typedef struct {
    /* Translation Id */
  uint32_t name;
  menuOptionType type;
  /* Translation Id */
  uint32_t baseOption;
  uint8_t *selectedOption;
  // bool animate;
  menuoptioncallback_t callback;
} menuoption_t;

typedef struct {
  int *selector;
  uint16_t optionCount;
  const menuoption_t *options;
  menuhelpcallback_t helpCallback;
} menu_t;


static inline void render_setting_row(volatile uint8_t *frame, const char *title, const char *value, uint16_t optcnt, uint8_t offy) {
    const unsigned rowh = 20;
    draw_text_ovf(title, frame, 8, offy + rowh * optcnt, 224);
    draw_central_text(value, frame, 170, offy + rowh * optcnt);
}

void renderMenu(volatile uint8_t *frame, const menu_t *menu) {
  char tmpbuf[TMP_BUF_SIZE];
  uint8_t numrows = (menu->helpCallback == NULL ? ROW_COUNT : ROW_COUNT - 1);
  // Odd number round up, e.g. 5 -> we want 3
  uint8_t halfnumrows = (numrows + 1 ) / 2;
  bool scroll = menu->optionCount >= numrows;

  unsigned int rowh = 20;
  
  uint8_t offy = (scroll ? 29 : 22);

  unsigned selector = *menu->selector;

  if (scroll && selector > halfnumrows)
    draw_central_text("⯅", frame, 120, 15);
  if (scroll && selector < menu->optionCount - halfnumrows)
    draw_central_text("⯆", frame, 120, 15 + 20 * numrows);//125);

  uint8_t baseopt;
  // If we are in the first half of the first page or there aren't enough rows to scroll
  if (selector < halfnumrows || !scroll)
    baseopt = 0;
  // If we are in the second half of the last page
  else if (menu->optionCount - selector < halfnumrows) 
    baseopt = menu->optionCount - halfnumrows;
  else 
    baseopt = selector - halfnumrows;

  // npf_snprintf(tmpbuf, sizeof(tmpbuf), " %u ", baseopt);
  // render_setting_row(frame, "baseopt", tmpbuf, 0);
  
  for (uint8_t offopt = 0; offopt < MIN(numrows, menu->optionCount); offopt++)
  {
    uint16_t curropt = baseopt + offopt;
    if (curropt >= menu->optionCount)
      break;
    const menuoption_t *selopt = &menu->options[baseopt + offopt];
    switch (selopt->type)
    {
    case Header:
      draw_central_text(msgs[lang_id][selopt->name], frame, SCREEN_WIDTH/2, offy + rowh*offopt);
      break;
    case TxtScroll:
      char *msg = msgs[lang_id][selopt->baseOption + (selopt->selectedOption == NULL ? 0 : *selopt->selectedOption)];
      if (msg[0] != '<'){
        npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %s >", 
          msg);
        render_setting_row(frame, msgs[lang_id][selopt->name], tmpbuf, offopt, offy);
      }
      else  {
        render_setting_row(frame, msgs[lang_id][selopt->name], msg, offopt, offy);
      }
      break;
    case IntScroll:
      npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %i >", *((uint8_t*) selopt->selectedOption));
      render_setting_row(frame, msgs[lang_id][selopt->name], tmpbuf, offopt, offy);
      break;
    case Bool:
      render_setting_row(frame, msgs[lang_id][selopt->name], 
        msgs[lang_id][(selopt->baseOption ? selopt->baseOption : MSG_KNOB_DISABLED)+ *((bool *) selopt->selectedOption)],
          offopt, offy);
      break;
    case InvBool:
      render_setting_row(frame, msgs[lang_id][selopt->name], 
        msgs[lang_id][(selopt->baseOption ? selopt->baseOption : MSG_KNOB_DISABLED) + 1 - *((bool *) selopt->selectedOption)],
          offopt, offy);
      break;
    case Callback:
      selopt->callback(tmpbuf, TMP_BUF_SIZE);
      render_setting_row(frame, msgs[lang_id][selopt->name], tmpbuf, offopt, offy);
      break;
    case Button:
      // Buttons handle their own highlighting in the button box rendering
      draw_button_box(frame, 20, 220, 132, 152, selector == (baseopt + offopt));
      draw_central_text(msgs[lang_id][selopt->name], frame, 120, 134);
      break;
    default:
      break;
    }
  }

  // Render the highlight bar if not on save
  if (menu->options[*menu->selector].type != Button)
    render_bar_fs(offy + (selector - baseopt) * rowh);
  if (menu->helpCallback != NULL)
    menu->helpCallback(frame);
}