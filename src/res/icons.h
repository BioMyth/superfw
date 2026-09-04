#pragma once
#ifndef ICONS_H
#define ICONS_H

#include "common.h"

enum {
 ICON_FOLDER,
 ICON_BINFILE,
 ICON_UPDFILE,
 ICON_GBCART,
 ICON_GBCCART,
 ICON_GBACART,
 ICON_SMSCART,
 ICON_NESCART,
 ICON_RECENT,
 ICON_DISK,
#ifdef SUPPORT_NORGAMES
 ICON_FLASH,
#endif
 ICON_SETTINGS,
 ICON_UILANG_SETTINGS,
 ICON_TOOLS,
 ICON_INFO,
 ICON_HFOLDER,
 ICON_HFILE,
};

extern const uint8_t icons_img[ICON_HFILE + 1][4][8][8];
extern const uint16_t icons_pal[38];
#endif
