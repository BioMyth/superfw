#pragma once

#include "menu_renderer.h"
#include "settings.h"
#include "menustate.h"

enum ToolsOptions_e {
  ToolSDRAMTest = 0,
  ToolSRAMTest,
  ToolBatteryTest,
  ToolSDBench,
  ToolFlashBak,
  #ifdef SUPPORT_NORGAMES
  ToolFlashClr,
  #endif
  ToolMAX
};

static const struct menu_row ToolsMenuOptions[] = {
  {
    .title_trans = MSG_TOOLS0_SDRAM,
    .type = MENU_ROW_TXT,
  },
  {
    .title_trans = MSG_TOOLS1_SRAM,
    .type = MENU_ROW_TXT,
  },
  {
    .title_trans = MSG_TOOLS2_BAT,
    .type = MENU_ROW_TXT,
  },
  {
    .title_trans = MSG_TOOLS3_BENCH,
    .type = MENU_ROW_TXT,
  },
  {
    .title_trans = MSG_TOOLS4_FBAK,
    .type = MENU_ROW_TXT,
  }
#ifdef SUPPORT_NORGAMES
    ,
  {
    .title_trans = MSG_TOOLS5_FCLR,
    .type = MENU_ROW_TXT,
  }
#endif
};

const struct menu toolsMenu = {
    .menu_sel = &smenu.tools.selector,
    .row_cnt = ToolMAX,
    .rows = ToolsMenuOptions
};