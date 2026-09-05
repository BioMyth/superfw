#pragma once

#include "menu_renderer.h"
#include "settings.h"
#include "menustate.h"

enum LoadOptOptions_e {
  LoadOptHeader,
  LoadOptSaveLoad,
  LoadOptSaveSave,
  LoadOptRTCTime,
  LoadOptCheats,
  LoadOptSave,
  LoadOptMax
};

static struct rtc_context {
    bool *enabled;
    uint32_t *val
};

static void rtc_callback (char *tmpbuf, const uint16_t buffsize, const void *context){
    if (context == NULL)
        return;
    struct rtc_context *rtc_data = (struct rtc_context *) context; 
    if (*rtc_data->enabled)
    {
        t_dec_date d;
        timestamp2date(
            *rtc_data->val, &d);
        npf_snprintf(tmpbuf, buffsize, "20%02d/%02d/%02d %02d:%02d",
                    d.year, d.month, d.day, d.hour, d.min);
    }
    else {
        strcpy(tmpbuf, "-");
    }
}

static struct menu_row LoadOptMenuOptions[LoadOptMax] = {
    {
        .title_trans = MSG_GBALOAD_OPTS,
        .type = MENU_ROW_HEADER,
    },
    {
        .title_trans = MSG_LOADER_LOADP,
        .type = MENU_ROW_TXT_SCROLL,
        .base_value_trans = MSG_LOADER_LOADP0,
        //.value_sel = (uint8_t*) &loadcfg->sram_load_type
    },
    {
        .title_trans = MSG_LOADER_SAVEP,
        .type = MENU_ROW_TXT_SCROLL,
        .base_value_trans = MSG_LOADER_SAVEP0,
        //.value_sel = (uint8_t*) &loadcfg->sram_save_type
    },
    {
        .title_trans = MSG_DEF_RTCVAL,
        .type = MENU_ROW_CUSTOM,
        .callback = rtc_callback,
        //.context = (void *) &(struct rtc_context) {
        //    .enabled = &spop->p.load.i.rtc_patch_enabled,
        //    .val = &loadcfg->rtcval
        //}
    },
    {
        .title_trans = MSG_SETT_LDCHT,
        .type = MENU_ROW_BOOL,
        //.value_sel = (uint8_t *)&loadcfg->use_cheats
    },
    {
        .title_trans = MSG_SETT_REMEMB,
        .type = MENU_ROW_BUTT,
    }
};
 

static inline void help_callback (char *tmp, uint16_t size, uint8_t selector, const void *context) {
    t_load_gba_lcfg *loadcfg = (t_load_gba_lcfg *) context;
    switch (selector)
    {
    case GBALdSetLoadP:
        strcpy(tmp, msgs[lang_id][MSG_LOADER_LOADP_I0 + loadcfg->sram_load_type]);
        break;
    case GBALdSetSaveP:
        strcpy(tmp,msgs[lang_id][MSG_LOADER_SAVEP_I0 + loadcfg->sram_save_type]);
        break;
    case GBALdRemember:
        strcpy(tmp, msgs[lang_id][MSG_REMEMB_I]);
        break;
    case GBALdSetCheats:
        if (!enable_cheats)
            strcpy(tmp, msgs[lang_id][MSG_CHEATSDIS_I]);
        else if (!loadcfg->cheats_found)
            strcpy(tmp, msgs[lang_id][MSG_CHEATSNOA_I]);
    default:
        break;
    }
}



static struct menu_row * initOpts(t_load_gba_lcfg *loadcfg, bool *rtc){
        return (struct menu_row [LoadOptMax]) {
        {
            .title_trans = MSG_GBALOAD_OPTS,
            .type = MENU_ROW_HEADER,
        },
        {
            .title_trans = MSG_LOADER_LOADP,
            .type = MENU_ROW_TXT_SCROLL,
            .base_value_trans = MSG_LOADER_LOADP0,
            .value_sel = (uint8_t*) &loadcfg->sram_load_type
        },
        {
            .title_trans = MSG_LOADER_SAVEP,
            .type = MENU_ROW_TXT_SCROLL,
            .base_value_trans = MSG_LOADER_SAVEP0,
            .value_sel = (uint8_t*) &loadcfg->sram_save_type
        },
        {
            .title_trans = MSG_DEF_RTCVAL,
            .type = MENU_ROW_CUSTOM,
            .callback = rtc_callback,
            .context = (void *) &(struct rtc_context) {
                .enabled = rtc,
                .val = &loadcfg->rtcval
            }
        },
        {
            .title_trans = MSG_SETT_LDCHT,
            .type = MENU_ROW_BOOL,
            .value_sel = (uint8_t *)&loadcfg->use_cheats
        },
        {
            .title_trans = MSG_SETT_REMEMB,
            .type = MENU_ROW_BUTT,
        }
    };
    
}
    
struct menu popLoad = {
        //.menu_sel = (uint8_t *) &spop->selector,
        .type = MENU_POPUP,
        .row_cnt = LoadOptMax,
        .rows = LoadOptMenuOptions,
        .bott_callback = help_callback
    };

// struct menu 
void initLoadOptMenu (struct popupstate *spop, bool isnor) {
    struct t_load_gba_lcfg *loadcfg;
    if (isnor)
        loadcfg = &spop->p.norld.l;
    else
        loadcfg = &spop->p.load.l;
    popLoad.menu_sel = &spop->selector;
    popLoad.rows[LoadOptSaveLoad].value_sel = (uint8_t*) &loadcfg->sram_load_type;
    popLoad.rows[LoadOptSaveSave].value_sel = (uint8_t*) &loadcfg->sram_save_type;
    // popLoad.rows[LoadOptRTCTime].context = (void *) &(struct rtc_context) {
    //                .enabled = &spop->p.load.i.rtc_patch_enabled,
    //                .val = &loadcfg->rtcval
    // };
    popLoad.rows[LoadOptCheats].value_sel = (uint8_t *)&loadcfg->use_cheats;


}