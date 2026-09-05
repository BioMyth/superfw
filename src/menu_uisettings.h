#pragma once

#include "menu_renderer.h"
#include "settings.h"
#include "menustate.h"

typedef enum UiSetEnum{
  UiSettTheme,
  UiSettLang,
  UiSettRect,
  UiSettASpd,
  UiSettHid,
  UiSettSave,
  UiSettMAX,
} UiSetEnum;

static const struct menu_row UiSetMenuOpts[] = {
  {
    .title_trans = MSG_UIS_THEME,
    .type = MENU_ROW_INT_SCROLL,
    .value_sel = &menu_theme
  },
  {
    .title_trans = MSG_UIS_LANG,
    .type = MENU_ROW_TXT_SCROLL,
    .base_value_trans = MSG_LANG_NAME
  },
  {
    .title_trans = MSG_UIS_RECNT,
    .type = MENU_ROW_BOOL,
    .value_sel = &recent_menu
  },
  {
    .title_trans = MSG_UIS_ANSPD,
    .type = MENU_ROW_TXT_SCROLL,
    .base_value_trans = MSG_UIS_SPD0,
    .value_sel = &anim_speed
  },
  {
    .title_trans = MSG_UIS_BHID,
    .type = MENU_ROW_BOOL_INV,
    .value_sel = &hide_hidden
  },
  {
    .title_trans = MSG_UIS_SAVE,
    .type = MENU_ROW_BUTT
  }
};

const struct menu uiSetMenu = {
  .menu_sel = (uint8_t *) &smenu.uiset.selector,
  .row_cnt = UiSettMAX,
  .rows = UiSetMenuOpts
};