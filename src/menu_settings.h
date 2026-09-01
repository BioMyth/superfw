#pragma once

#include "menustate.h"
#include "common.h"
#include "settings.h"
#include "nanoprintf.h"
#include "messages.h"
#include "renderEngine.h"


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
  SettMAX = SettSave
};

static void  rtcRenderCallback(char *tmpbuf, uint16_t buffsize){
        t_dec_date d;
        timestamp2date(rtcvalue_default, &d);
        npf_snprintf(tmpbuf, buffsize, "20%02d/%02d/%02d %02d:%02d",
          d.year, d.month, d.day, d.hour, d.min);
}

static void rtcSpeedRenderCallback(char *tmpbuf, uint16_t buffsize) {
  unsigned spdmsg = rtcspeed_default ? (MSG_UIS_SPD0 + rtcspeed_default - 1) : MSG_STILLRTC;
  npf_snprintf(tmpbuf, buffsize, "< %s >", msgs[lang_id][spdmsg]);
}

static void hotkeyRenderCallback(char *tmpbuf, uint16_t buffsize){
      npf_snprintf(tmpbuf, buffsize, "< %s >", hotkey_list[hotkey_combo].cname);
}

static void savePathRenderCallback(char *tmpbuf, uint16_t buffsize){
  if (save_path_default == SaveRomName)
    npf_snprintf(tmpbuf, buffsize, "< %s >", msgs[lang_id][MSG_NEXTTO_ROM]);
  else
    npf_snprintf(tmpbuf, buffsize, "< %s >", save_paths[save_path_default]);
}

static void savePathFlashRenderCallback(char *tmpbuf, uint16_t buffsize) {
  npf_snprintf(tmpbuf, buffsize, "< %s >", save_paths[save_path_nor_default]);
}

static void saveStatePathRenderCallback(char *tmpbuf, uint16_t buffsize){
  npf_snprintf(tmpbuf, buffsize, "< %s >", savestates_paths_display[state_path_default]);
}



static const menuoption_t SetMenuOpts[] = {
  {
    MSG_SET_TITL1,
    Header,
    0,
    NULL,
    // false,
    NULL
  },
  {
    MSG_SETT_HOTK,
    Callback,
    0,
    NULL,
    // false,
    hotkeyRenderCallback
  },
  {
    MSG_SETT_BOOT,
    TxtScroll,
    MSG_BOOT_TYPE0,
    &boot_bios_splash,
    // false,
    NULL
  },
  {
    MSG_SETT_FASTSD,
    InvBool,
    0,
    &use_slowld,
    // false,
    NULL
  },
  #ifdef SUPPORT_NORGAMES
  {
    MSG_SETT_VERNOR,
    Bool,
    0,
    &use_verify_nor,
    // false,
    NULL
  },
  #endif
  {
    MSG_SETT_FASTEW,
    Bool,
    0,
    &use_fastew,
    // false,
    NULL
  },
  {
    MSG_SETT_SAVET,
    Callback,
    0,
    NULL,
    // false,
    savePathRenderCallback
  },
  #ifdef SUPPORT_NORGAMES
  {
    MSG_SETT_SAVETX,
    Callback,
    0,
    NULL,
    // false,
    savePathFlashRenderCallback
  },
  #endif
  {
    MSG_SETT_SAVEBK,
    IntScroll,
    0,
    &backup_sram_default,
    // false,
    NULL
  },
  {
    MSG_SETT_STATET,
    Callback,
    0,
    NULL,
    // false,
    saveStatePathRenderCallback
  },
  {
    MSG_SETT_CHTEN,
    Bool,
    0,
    &enable_cheats,
    // false,
    NULL
  },
  {
    MSG_SET_TITL2,
    Header,
    0,
    NULL,
    // false,
    NULL
  },
  {
    MSG_DEFS_PATCH,
    TxtScroll,
    MSG_PATCH_TYPE0,
    &patcher_default,
    // false,
    NULL
  },
  {
    MSG_LOADER_MENU,
    Bool,
    0,
    &ingamemenu_default,
    // false,
    NULL
  },
  {
    MSG_LOADER_RTCE,
    Bool,
    0,
    &rtcpatch_default,
    // false,
    NULL
  },
  {
    MSG_DEF_RTCVAL,
    Callback,
    0,
    NULL,
    // false,
    rtcRenderCallback
  },
  {
    MSG_DEF_SPEED,
    Callback,
    0,
    NULL,
    // false,
    rtcSpeedRenderCallback
  },
  {
    MSG_LOADER_LOADP,
    InvBool,
    MSG_DEF_LOADP0,
    &autoload_default,
    // false,
    NULL
  },
  {
    MSG_LOADER_SAVEP,
    Bool,
    MSG_DEF_SAVEP0,
    &autosave_default,
    // false,
    NULL
  },
  {
    MSG_LOADER_PREFDS,
    Bool,
    0,
    &autosave_prefer_ds,
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



void uiSetRenderHelp(volatile uint8_t *frame) {
  char tmp[128];
  // Render bar below for help messge
  dma_memset16(&frame[240*140], dup8(FG_COLOR), 240*20/2);

  if (smenu.set.selector == SettSaveLoc) {
    if (save_path_default == SaveRomName)
      draw_text_ovf_rotate(msgs[lang_id][MSG_SAVE_TYPE_NR], frame, 4, SCREEN_HEIGHT - 18, 232, &smenu.anim_state);
    else {
      npf_snprintf(tmp, sizeof(tmp), msgs[lang_id][MSG_SAVE_TYPE_PT], save_paths[save_path_default]);
      draw_text_ovf_rotate(tmp, frame, 4, SCREEN_HEIGHT - 18, 232, &smenu.anim_state);
    }
  }
  else if (smenu.set.selector == SettStateLoc) {
    npf_snprintf(tmp, sizeof(tmp), msgs[lang_id][MSG_STATE_TYPE_PT], savestates_paths[state_path_default]);
    draw_text_ovf_rotate(tmp, frame, 4, SCREEN_HEIGHT - 18, 232, &smenu.anim_state);
  }
  #ifdef SUPPORT_NORGAMES
  else if (smenu.set.selector == SettSaveLocNOR) {
    npf_snprintf(tmp, sizeof(tmp), msgs[lang_id][MSG_SAVE_TYPE_PTX], save_paths[save_path_nor_default]);
    draw_text_ovf_rotate(tmp, frame, 4, SCREEN_HEIGHT - 18, 232, &smenu.anim_state);
  }
  #endif
  else {
    unsigned help_msg = smenu.set.selector == SettBootType ? MSG_BOOT_TYPE_I0 + boot_bios_splash :
                        smenu.set.selector == SettSaveBkp  ? MSG_BACKUP_I :
                        smenu.set.selector == SettFastSD   ? MSG_FASTSD_I :
                        smenu.set.selector == SettFastEWRAM? MSG_FASTEW_I :
                        smenu.set.selector == DefsPatchEng ? MSG_PATCH_TYPE_I0 + patcher_default :
                        smenu.set.selector == DefsLoadPol  ? MSG_DEF_LOADP_I0 + (autoload_default ^ 1) :
                        smenu.set.selector == DefsSavePol  ? MSG_DEF_SAVEP_I0 + (autosave_default ^ 1) :
                        smenu.set.selector == DefsPrefDS   ? MSG_LOADER_PREFDSI :
                        #ifdef SUPPORT_NORGAMES
                        smenu.set.selector == SettVerifyNOR ? MSG_VERNOR_I :
                        #endif
                        MSG_EMPTY;
    draw_text_ovf_rotate(msgs[lang_id][help_msg], frame, 4, SCREEN_HEIGHT - 18, 232, &smenu.anim_state);
  }
}

const menu_t globalSetMenu = {
      .selector = &smenu.set.selector,
      .optionCount = SettMAX,
      .options = SetMenuOpts,
      .helpCallback = uiSetRenderHelp
};
