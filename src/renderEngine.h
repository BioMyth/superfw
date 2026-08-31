#pragma once

#include "gbahw.h"
#include "common.h"
#include "messages.h"
#include "drawutils.h"

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

typedef enum menuOptionType {
  Header,
  RTC,
  IntScroll,
  TxtScroll,
  ArrScroll,
  Bool,
  InvBool,
  HotKey,
  Callback,
  Save
} menuOptionType;

typedef void (*menuoptioncallback)(char *tmpbuf);

typedef struct menuoption {
    /* Translation Id */
  uint32_t name;
  menuOptionType type;
  /* Translation Id */
  uint32_t baseOption;
  uint8_t *selectedOption;
  // bool animate;
  menuoptioncallback callback;
} menuoption;

typedef struct menu {
  int *selector;
  uint16_t optionCount;
  menuoption *options;
  void (*helpCallback)(volatile uint8_t *frame);
} menu;


static inline void render_setting_row(volatile uint8_t *frame, const char *title, const char *value, uint16_t optcnt, uint8_t offy) {
    const unsigned rowh = 20;
    draw_text_ovf(title, frame, 8, offy + rowh * optcnt, 224);
    draw_central_text(value, frame, 170, offy + rowh * optcnt);
}

void renderMenu(volatile uint8_t *frame, const menu *menu) {
  char tmpbuf[128];
  uint8_t numrows = (menu->helpCallback == NULL ? ROW_COUNT : ROW_COUNT - 1);
  bool scroll = menu->optionCount >= numrows;

  unsigned int rowh = 20;
  
  uint8_t offy = (scroll ? 29 : 22);

  unsigned selector = *menu->selector;

  if (scroll && selector > numrows/2)
    draw_central_text("⯅", frame, 120, 15);
  if (scroll && selector < menu->optionCount - numrows/2)
    draw_central_text("⯆", frame, 120, 15 + 20 * numrows);//125);

  uint8_t baseopt;
  // If we are in the first half of the first page or there aren't enough rows to scroll
  if (selector < numrows/2 || !scroll)
    baseopt = 0;
  // If we are in the second half of the last page
  else if (menu->optionCount - selector < numrows/2) 
    baseopt = menu->optionCount - numrows/2;
  else 
    baseopt = selector - numrows/2;

  // npf_snprintf(tmpbuf, sizeof(tmpbuf), " %u ", baseopt);
  // render_setting_row(frame, "baseopt", tmpbuf, 0);
  
  for (uint8_t offopt = 0; offopt < MIN(numrows, menu->optionCount); offopt++)
  {
      menuoption *selopt = &menu->options[baseopt + offopt];
      switch (selopt->type)
      {
      case Header:
        draw_central_text(msgs[lang_id][selopt->name], frame, SCREEN_WIDTH/2, offy + rowh*offopt);
        break;
      case TxtScroll:
        npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %s >", 
          msgs[lang_id][selopt->baseOption + (selopt->selectedOption == NULL ? 0 : *selopt->selectedOption)]);
          render_setting_row(frame, msgs[lang_id][selopt->name], tmpbuf, offopt, offy);
        break;
      case IntScroll:
        npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %i >", *((uint8_t*) selopt->selectedOption));
        render_setting_row(frame, msgs[lang_id][selopt->name], tmpbuf, offopt, offy);
        break;
      case Bool:
        render_setting_row(frame, msgs[lang_id][selopt->name], 
          msgs[lang_id][(selopt->baseOption ? selopt->baseOption : MSG_KNOB_DISABLED)+ *((bool *) selopt->selectedOption)],
         // msgs[lang_id][(*((bool *) selopt->selectedOption) ? MSG_KNOB_ENABLED : MSG_KNOB_DISABLED)],
           offopt, offy);
        break;
      case InvBool:
        render_setting_row(frame, msgs[lang_id][selopt->name], 
          msgs[lang_id][(selopt->baseOption ? selopt->baseOption : MSG_KNOB_DISABLED) + 1 - *((bool *) selopt->selectedOption)],
          // msgs[lang_id][(*((bool *) selopt->selectedOption) ? MSG_KNOB_DISABLED : MSG_KNOB_ENABLED)],
           offopt, offy);
        break;
      case Callback:
        selopt->callback(tmpbuf);
        render_setting_row(frame, msgs[lang_id][selopt->name], tmpbuf, offopt, offy);
        break;
      case Save:
        // draw_button_box(frame, 20, 220, 132, 152, selector == (baseopt + offopt));
        draw_central_text(msgs[lang_id][MSG_UIS_SAVE], frame, 120, 134);
        break;
      default:
        break;
      }
  }

  // Render the highlight bar if not on save
  // if (smenu.uiset.selector != UiSetSave)
    // #pragma GCC unroll 15
    for (unsigned i = 0; i < 15; i++)
      render_icon_trans(i * 16, offy + (selector - baseopt) * rowh, 63);
  if (menu->helpCallback != NULL)
    menu->helpCallback(frame);
}