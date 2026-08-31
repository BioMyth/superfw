#pragma once

#include "common.h"
#include "util.h"
#include "flash_mgr.h"
#include "patchengine.h"


typedef struct t_load_gba_info {
  // ROM information
  char romfn[MAX_FN_LEN];             // File to load/write
  uint32_t romfs;                     // File ROM size
  char gcode[5];                      // ASCII sanitized game code.
  t_rom_header romh;                  // ROM header (for info purposes)
  // Patching info
  t_patch patches_datab;              // Loaded patches (from DB)
  t_patch patches_cache;              // Loaded patches (from patch engine's cache)
  bool patches_datab_found;           // Whether we had a patch match in the database
  bool patches_cache_found;           // Same but for the patch cache
  // Patching configuration
  t_patch_policy patch_type;          // Patching type
  bool use_dsaving;                   // Whether we use direct-saving mode
  bool ingame_menu_enabled;           // Enable the in-game menu.
  bool rtc_patch_enabled;             // Patch for RTC workarounds.
} t_load_gba_info;

typedef struct t_load_gba_lcfg {
  // Save read/write policies and info
  t_sram_load_policy sram_load_type;  // SRAM loading policy
  t_sram_save_policy sram_save_type;  // SRAM auto-saving policy
  char savefn[MAX_FN_LEN];            // Save file path.
  bool savefile_found;                // Whether there's a .sav file.
  // RTC config
  uint32_t rtcval;                    // Initial RTC value.
  // Cheats policy
  bool use_cheats;                    // Whether we want to load cheats to use them.
  bool cheats_found;                  // Whether there's a cheats file (not parsed tho!)
  unsigned cheats_size;               // Size of the cheat buffer
  char cheatsfn[MAX_FN_LEN];          // Cheats file path.
} t_load_gba_lcfg;



typedef void (*t_mrender_fn)(volatile uint8_t *frame);
typedef void (*t_mkeyupd_fn)(unsigned newkeys, uint16_t deltaframes);

// Info and state for the menu tab
struct menustate{
  uint8_t menu_tab;

  unsigned anim_state;            // Animation (text rotation) status.

  // Recent ROMs state
  struct {
    int selector;                 // Pointed file offset
    int seloff;                   // Entry at the top of the list
    int maxentries;               // Total file/dir count in current dir
  } recent;

  // ROM browser state
  struct {
    char cpath[MAX_FN_LEN];       // Current path
    int selector;                 // Pointed file offset
    int seloff;                   // Entry at the top of the list
    int maxentries;               // Total file/dir count in current dir
    int dispentries;              // Maximum number of visible entries (filtered)
    uint16_t selhist[16];         // History of directory offsets
  } browser;

  // Flash ROM browser state
  struct {
    int selector;                 // Pointed file offset
    int seloff;                   // Entry at the top of the list
    uint8_t maxentries;           // Total file/dir count in current dir
    uint8_t usedblks, freeblks;   // NOR usage info
  } fbrowser;

  // UI settings
  struct {
    int selector;                 // Pointed option
  } uiset;

  // Main settings
  struct {
    int selector;                 // Pointed option
  } set;

  // Tools menu
  struct {
    int selector;                 // Render panel
  } tools;

  // Info/About menu
  struct {
    int selector;                 // Render panel
    char tstr[64];                // Temp message render
  } info;
} smenu;

// Same but for popups.
struct popupstate{
  const char *alert_msg;          // Extra pop-up message

  uint8_t pop_num;                // Current pop-up in display
  char submenu;                   // Which submenu tab we are in (if any)
  char selector;                  // Option selector (if any)
  unsigned anim;                  // Animation state

  // Pop up message (for whatever action). Allows returning to previous popup.
  struct {
    const char *message;
    const char *default_button;
    const char *confirm_button;
    void (*callback)(bool confirm);       // Function to call on "confirm".
    uint8_t option;                       // Selected button
    bool clear_popup_ok;                  // Whether any pop up must be cleared.
  } qpop;

  // RTC time set pop up, a bit special.
  struct {
    t_dec_date val;
    int selector;
    void (*callback)();                   // Function to call on "save"
  } rtcpop;

  union {
    // GBA launch ROM pop up menu
    struct {
      t_load_gba_info i;                  // ROM/Patch info and patch policy.
      t_load_gba_lcfg l;                  // ROM loading info and settings;
    } load;

    // Write GBA game to NOR memory
    struct {
      t_load_gba_info i;                  // ROM/Patch info and patch policy.
    } norwr;
    // Launch GBA game from NOR memory
    struct {
      t_load_gba_lcfg l;                  // ROM loading info and settings;
      const t_flash_game_entry *e;        // NOR game entry on RAM
    } norld;

    // Save file menu (.sav files)
    struct {
      char savfn[MAX_FN_LEN];             // SAV file to load/store/mangle
    } savopt;
    // Update menu (for .fw files)
    struct {
      char fn[MAX_FN_LEN];                // FW file to load and flash
      bool issfw;                         // The firmware is a superFW image.
      uint32_t superfw_ver;               // Reported FW version.
      uint32_t fw_size;                   // Size in bytes reported by stat.
      unsigned curr_state;                // Flashing FSM state.
    } update;

    // Not really a pop up, but used as "popup" data for menu questions.
    struct {
      char fn[MAX_FN_LEN];
      unsigned fs;
    } pdb_ld;
  } p;
} spop;
