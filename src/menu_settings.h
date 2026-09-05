#pragma once

#include "menustate.h"
#include "common.h"
#include "settings.h"
#include "nanoprintf.h"
#include "menu_renderer.h"


enum settingsOptions{
  SettTitle1 = 0,
  SettHotkey,
  SettBootType,
  SettFastSD,
  #ifdef SUPPORT_NORGAMES
  SettVerifyNOR,
  #endif
  SettFastEWRAM,
  SettSaveLoc,
  #ifdef SUPPORT_NORGAMES
  SettSaveLocNOR,
  #endif
  SettSaveBkp,
  SettStateLoc,
  SettCheatEn,
  SettTitle2,
  DefsPatchEng,
  DefsGamMenu,
  DefsRTCEnb,
  DefsRTCVal,
  DefsRTCSpeed,
  DefsLoadPol,
  DefsSavePol,
  DefsPrefDS,
  SettSave,
  SettMAX = SettSave - 2,


};

static void  rtcRenderCallback(char *tmpbuf, const uint16_t buffsize, const void *context){
        t_dec_date d;
        timestamp2date(rtcvalue_default, &d);
        npf_snprintf(tmpbuf, buffsize, "20%02d/%02d/%02d %02d:%02d",
          d.year, d.month, d.day, d.hour, d.min);
}

static void rtcSpeedRenderCallback(char *tmpbuf, const uint16_t buffsize, const void *context) {
  unsigned spdmsg = rtcspeed_default ? (MSG_UIS_SPD0 + rtcspeed_default - 1) : MSG_STILLRTC;
  npf_snprintf(tmpbuf, buffsize, "< %s >", msgs[lang_id][spdmsg]);
}

static void hotkeyRenderCallback(char *tmpbuf, const uint16_t buffsize, const void *context){
      npf_snprintf(tmpbuf, buffsize, "< %s >", hotkey_list[hotkey_combo].cname);
}

static void savePathRenderCallback(char *tmpbuf, const uint16_t buffsize, const void *context){
  if (save_path_default == SaveRomName)
    npf_snprintf(tmpbuf, buffsize, "< %s >", msgs[lang_id][MSG_NEXTTO_ROM]);
  else
    npf_snprintf(tmpbuf, buffsize, "< %s >", save_paths[save_path_default]);
}

static void savePathFlashRenderCallback(char *tmpbuf, const uint16_t buffsize, const void *context) {
  npf_snprintf(tmpbuf, buffsize, "< %s >", save_paths[save_path_nor_default]);
}

static void saveStatePathRenderCallback(char *tmpbuf, const uint16_t buffsize, const void *context){
  npf_snprintf(tmpbuf, buffsize, "< %s >", savestates_paths_display[state_path_default]);
}



static const struct menu_row SetMenuOpts[] = {
  {
    .title_trans = MSG_SET_TITL1,
    .type = MENU_ROW_HEADER,
  },
  {
    .title_trans = MSG_SETT_HOTK,
    .type = MENU_ROW_CUSTOM,
    .callback = hotkeyRenderCallback
  },
  {
    .title_trans = MSG_SETT_BOOT,
    .type = MENU_ROW_TXT_SCROLL,
    .base_value_trans = MSG_BOOT_TYPE0,
    .value_sel = &boot_bios_splash
  },
  {
    .title_trans = MSG_SETT_FASTSD,
    .type = MENU_ROW_BOOL_INV,
    .value_sel = &use_slowld
  },
  #ifdef SUPPORT_NORGAMES
  {
    .title_trans = MSG_SETT_VERNOR,
    .type = MENU_ROW_BOOL,
    .value_sel = &use_verify_nor
  },
  #endif
  {
    .title_trans = MSG_SETT_FASTEW,
    .type = MENU_ROW_BOOL,
    .value_sel = &use_fastew,
  },
  {
    .title_trans = MSG_SETT_SAVET,
    .type = MENU_ROW_CUSTOM,
    .callback = savePathRenderCallback
  },
  #ifdef SUPPORT_NORGAMES
  {
    .title_trans = MSG_SETT_SAVETX,
    .type = MENU_ROW_CUSTOM,
    .callback = savePathFlashRenderCallback
  },
  #endif
  {
    .title_trans = MSG_SETT_SAVEBK,
    .type = MENU_ROW_INT_SCROLL,
    .value_sel = &backup_sram_default
  },
  {
    .title_trans = MSG_SETT_STATET,
    .type = MENU_ROW_CUSTOM,
    .callback = saveStatePathRenderCallback
  },
  {
    .title_trans = MSG_SETT_CHTEN,
    .type = MENU_ROW_BOOL,
    .value_sel = &enable_cheats
  },
  {
    .title_trans = MSG_SET_TITL2,
    .type = MENU_ROW_HEADER
  },
  {
    .title_trans = MSG_DEFS_PATCH,
    .type = MENU_ROW_TXT_SCROLL,
    .base_value_trans = MSG_PATCH_TYPE0,
    .value_sel = &patcher_default
  },
  {
    .title_trans = MSG_LOADER_MENU,
    .type = MENU_ROW_BOOL,
    .value_sel = &ingamemenu_default
  },
  {
    .title_trans = MSG_LOADER_RTCE,
    .type = MENU_ROW_BOOL,
    .value_sel = &rtcpatch_default
  },
  {
    .title_trans = MSG_DEF_RTCVAL,
    .type = MENU_ROW_CUSTOM,
    .callback = rtcRenderCallback
  },
  {
    .title_trans = MSG_DEF_SPEED,
    .type = MENU_ROW_CUSTOM,
    .callback = rtcSpeedRenderCallback
  },
  {
    .title_trans = MSG_LOADER_LOADP,
    .type = MENU_ROW_BOOL_INV,
    .base_value_trans = MSG_DEF_LOADP0,
    .value_sel = &autoload_default
  },
  {
    .title_trans = MSG_LOADER_SAVEP,
    .type = MENU_ROW_BOOL,
    .base_value_trans = MSG_DEF_SAVEP0,
    .value_sel = &autosave_default
  },
  {
    .title_trans = MSG_LOADER_PREFDS,
    .type = MENU_ROW_BOOL,
    .value_sel = &autosave_prefer_ds
  },
  {
    .title_trans = MSG_UIS_SAVE,
    .type = MENU_ROW_BUTT
  }
};

// TODO: Make settings a struct type or some BS & provide via context here
void uiSetHelpCallback(char *tmp, uint16_t size, uint8_t selector, const void *context) {
  if (selector == SettSaveLoc) {
    if (save_path_default == SaveRomName)
      strcpy(tmp, msgs[lang_id][MSG_SAVE_TYPE_NR]);
    else
      npf_snprintf(tmp, size, msgs[lang_id][MSG_SAVE_TYPE_PT], save_paths[save_path_default]);
  }
  else if (selector == SettStateLoc) {
    npf_snprintf(tmp, size, msgs[lang_id][MSG_STATE_TYPE_PT], savestates_paths[state_path_default]);
  }
  #ifdef SUPPORT_NORGAMES
  else if (selector == SettSaveLocNOR) {
    npf_snprintf(tmp, size, msgs[lang_id][MSG_SAVE_TYPE_PTX], save_paths[save_path_nor_default]);
  }
  #endif
  else {
    enum TranslationID help_trans;
      if( selector == SettBootType){
       help_trans = MSG_BOOT_TYPE_I0 + boot_bios_splash;
      } else {
        switch (selector)
        {
        case SettSaveBkp:
          help_trans = MSG_BACKUP_I;
          break;
        case SettFastSD:
          help_trans = MSG_FASTSD_I;
          break;
        case SettFastEWRAM:
          help_trans = MSG_FASTEW_I;
          break;
        case DefsPrefDS:
          help_trans = MSG_LOADER_PREFDSI;
          break;
        #ifdef SUPPORT_NORGAMES
        case SettVerifyNOR:
          help_trans = MSG_VERNOR_I;
          break;
        #endif
        case DefsPatchEng:
          help_trans = MSG_PATCH_TYPE_I0 + patcher_default;
          break;
        case DefsLoadPol:
          help_trans = MSG_DEF_LOADP_I0 + (autoload_default ^ 1);
          break;
        case DefsSavePol:
          help_trans = MSG_DEF_SAVEP_I0 + (autosave_default ^ 1);
          break;
        default:
          help_trans = MSG_EMPTY;
          break;
        }
      }
    strcpy(tmp, msgs[lang_id][help_trans]);
  }
}

const struct menu globalSetMenu = {
  .menu_sel = (uint8_t *) &smenu.set.selector,
  .row_cnt = sizeof(SetMenuOpts) / sizeof(struct menu_row),
  .rows = SetMenuOpts,
  .bott_callback = uiSetHelpCallback
};
