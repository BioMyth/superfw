#pragma once

#include "messages.h"
#include "renderEngine.h"
#include "settings.h"
#include "menustate.h"

typedef enum UiSetEnum{
  UiSetTheme = 0,
  UiSetLang  = 1,
  UiSetRect  = 2,
  UiSetASpd  = 3,
  UiSetHid   = 4,
  UiSetSave  = 5,
  UiSetMAX   = 5,
} UiSetEnum;

static const menuoption_t UiSetMenuOpts[] = {
  {
    MSG_UIS_THEME,
    IntScroll,
    0,
    &menu_theme,
    // false,
    NULL
  },
  {
    MSG_UIS_LANG,
    TxtScroll,
    MSG_LANG_NAME,
    NULL,
    // false,
    NULL
  },
  {
    MSG_UIS_RECNT,
    Bool,
    0,
    &recent_menu,
    // false,
    NULL
  },
  {
    MSG_UIS_ANSPD,
    TxtScroll,
    MSG_UIS_SPD0,
    &anim_speed,
    // false,
    NULL
  },
  {
    MSG_UIS_BHID,
    InvBool,
    0,
    &hide_hidden,
    // false,
    NULL
  },
  {
    MSG_UIS_SAVE,
    Button,
    0,
    NULL,
    // false,
    NULL
  }
};

const menu_t uiSetMenu = {
      .selector = &smenu.uiset.selector,
      .optionCount = UiSetMAX,
      .options = UiSetMenuOpts,
      .helpCallback = NULL
};