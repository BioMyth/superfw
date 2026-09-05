/*
* Copyright (C) 2026 Samuel Casteel <david@davidgf.net>
*
* This program is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see
* <http://www.gnu.org/licenses/>.
*/
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

#define SPRITE_SIZE 16

#define TMP_BUF_SIZE 128
#define TOP_BAR_HEIGHT 16
#define SUBMENU_BORDER 2

#define MENU_MARGIN 8
/*
* Types allowed for menu rows
*/
enum MenuRowType {
  MENU_ROW_BLANK,
  MENU_ROW_BOOL,
  MENU_ROW_BOOL_INV,
  MENU_ROW_BUTT,
  MENU_ROW_CUSTOM,
  MENU_ROW_HEADER,
  MENU_ROW_INT_SCROLL,
  MENU_ROW_SUB_HEADER,
  MENU_ROW_TXT,
  MENU_ROW_TXT_SCROLL,
  MENU_ROW_RTC,
};
/*
* Types of menus available
*/
enum MenuType {
  MENU_TAB,
  MENU_POPUP
};

/*
* Tabs on the top of the screen
*/
enum MenuTab {
  MENU_TAB_RECENT = 0,
  MENU_TAB_ROM_BROWSE,
  #ifdef SUPPORT_NORGAMES
  MENU_TAB_FLASH_BROWSE,
  #endif
  MENU_TAB_SETT,
  MENU_TAB_UI_SETT,
  MENU_TAB_TOOLS,
  MENU_TAB_INFO,
  MENU_TAB_MAX
};

enum TextAlign {
  TEXT_ALIGN_RIGHT,
  TEXT_ALIGN_LEFT,
  TEXT_ALIGN_CENTER
};

// typedef void (menuopt_func)(char *tmpbuf, uint16_t bufsize);
// typedef void (menubottcallback)(char *tmpbuf, uint8_t selectedOption);

typedef void (custommenurow_t)(char *tmp, const uint16_t size, const void *context);
typedef void (menubottcallback_t)(char *tmp, const uint16_t size, const uint8_t selector, const void *context);

struct menu_row{
  const enum TranslationID title_trans;
  const enum MenuRowType type;
  union 
  {
    const enum TranslationID base_value_trans;
    void *context;
  };
  
  // Union to reduce memory footprint since these are mutually exclusive
  union
  {
    custommenurow_t *callback;
    uint8_t *value_sel;
  };
  
};

struct menu{
  menubottcallback_t *bott_callback;
  void *context;
  uint8_t *menu_sel;
  struct menu_row *rows;
  uint8_t row_cnt;
  const enum MenuType type;
};

// inline void render_setting_row(
//   volatile uint8_t *frame_buf,
//   const char *title, const char *value,
//   uint8_t x, uint8_t y
// );

void render_screen(volatile uint8_t *frame, 
  const uint8_t tab,
  const enum MenuTab min_tab);

void render_menu(
  volatile uint8_t *frame, 
  unsigned *anim_state, 
  const struct menu *menu
);
