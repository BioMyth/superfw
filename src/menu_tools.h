#pragma once

#include "messages.h"
#include "renderEngine.h"
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

static const menuoption_t ToolsMenuOptions[] = {
  {
    .name = MSG_TOOLS0_SDRAM,
    .type = SelectableRow
  },
  {
    .name = MSG_TOOLS1_SRAM,
    .type = SelectableRow
  },
  {
    .name = MSG_TOOLS2_BAT,
    .type = SelectableRow
  },
  {
    .name = MSG_TOOLS3_BENCH,
    .type = SelectableRow
  },
  {
    .name = MSG_TOOLS4_FBAK,
    .type = SelectableRow
  }
#ifdef SUPPORT_NORGAMES
    ,
  {
    .name = MSG_TOOLS5_FCLR,
    .type = SelectableRow
  }
#endif
};

const menu_t toolsMenu = {
    .selector = &smenu.tools.selector,
    .optionCount = ToolMAX,
    .options = ToolsMenuOptions
};