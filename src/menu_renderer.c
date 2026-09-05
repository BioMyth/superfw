

/*
  See menu_renderer.h
*/
#include "menu_renderer.h"

static void render_scrollable_text (char *tmpbuf, volatile uint8_t *frame_buf, 
  const uint8_t x, const uint8_t y, const uint8_t maxw,
  unsigned *anim_state) {
    const unsigned twidth = font_width(tmpbuf);
    // Render rotating text if exceeding max width & max width isn't 0
    if (twidth > maxw && maxw)
      draw_text_ovf_rotate(tmpbuf, frame_buf, x, y, maxw, anim_state);
    else
      // Center the x value for the text on the reserved text space
      draw_central_text_ovf(tmpbuf, frame_buf, x + maxw / 2, y, maxw);
}

static void render_setting_row(volatile uint8_t *frame_buf, 
  const char *title, const char *value, 
  const uint8_t x, const uint8_t y) {
  draw_text_ovf(title, frame_buf, x, y, SCREEN_WIDTH - 2*x);
  draw_central_text(value, frame_buf, 170, y);
}


static void render_menu_option(volatile uint8_t *frame_buf, 
  const unsigned anim_state, const struct menu_row *row, 
  const bool selected, 
  const uint8_t x, const uint8_t y) {

  char tmpbuf[TMP_BUF_SIZE];
  switch (row->type)
  {
  case MENU_ROW_SUB_HEADER:
    draw_text_ovf("⯇", frame_buf, x, y, 64);
    draw_rightj_text("⯈", frame_buf, SCREEN_WIDTH - x, y);
  case MENU_ROW_HEADER:
    draw_central_text(msgs[lang_id][row->title_trans], frame_buf, SCREEN_WIDTH/2, y);
    break;
  case MENU_ROW_TXT_SCROLL:
    const char *msg = msgs[lang_id][row->base_value_trans + (row->value_sel == NULL ? 0 : *row->value_sel)];
    if (msg[0] != '<'){
      npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %s >", 
        msg);
      render_setting_row(frame_buf, msgs[lang_id][row->title_trans], tmpbuf, x, y);
    }
    else  {
      render_setting_row(frame_buf, msgs[lang_id][row->title_trans], msg, x, y);
    }
    break;
  case MENU_ROW_INT_SCROLL:
    npf_snprintf(tmpbuf, sizeof(tmpbuf), "< %i >", *((uint8_t*) row->value_sel));
    render_setting_row(frame_buf, msgs[lang_id][row->title_trans], tmpbuf, x, y);
    break;
  case MENU_ROW_BOOL:
    render_setting_row(frame_buf, msgs[lang_id][row->title_trans], 
      msgs[lang_id][(row->base_value_trans ? row->base_value_trans : MSG_KNOB_DISABLED)+ *((bool *) row->value_sel)], x, y);
    break;
  case MENU_ROW_BOOL_INV:
    render_setting_row(frame_buf, msgs[lang_id][row->title_trans], 
      msgs[lang_id][(row->base_value_trans ? row->base_value_trans : MSG_KNOB_DISABLED) + 1 - *((bool *) row->value_sel)],
      x, y);
    break;
  case MENU_ROW_CUSTOM:
    row->callback(tmpbuf, TMP_BUF_SIZE, row->context);
    render_setting_row(frame_buf, msgs[lang_id][row->title_trans], tmpbuf, x, y);
    break;
  case MENU_ROW_BUTT:
    // Buttons handle their own highlighting in the button box rendering
    draw_button_box(frame_buf, 20, SCREEN_WIDTH - 20,
      y, y + ROW_HEIGHT,
      selected);
    draw_central_text(msgs[lang_id][row->title_trans], frame_buf, SCREEN_WIDTH / 2, y + 2);
    break;
  case MENU_ROW_TXT:
    draw_text_ovf(msgs[lang_id][row->title_trans], frame_buf, 22, y, 144);
    if (selected) {
      draw_central_text("▸", frame_buf, MENU_MARGIN + ((anim_state & 0xFF) >> 6), y);
    }
    break;
  default:
    break;
  }
}

// TODO: Rename to render menu
void render_screen(volatile uint8_t *frame, 
  const uint8_t tab, 
  const enum MenuTab min_tab) {

  reset_sprites();

  // Render the tab menu on top (rows 0..15), highlighting the selected option
  render_bar(&frame[0], dup8((uint8_t) MENU_COLOR_FG), 0, 0, SCREEN_WIDTH, SPRITE_SIZE);

  // Render icon bar
  for (unsigned i = min_tab; i < MENU_TAB_MAX; i++)
    if (i == tab)
      render_icon((i - min_tab) * 16, 0, i + ICON_RECENT);
    else
      render_icon_trans((i - min_tab) * 16, 0, i + ICON_RECENT);

  // Render the main area
  dma_memset16(&frame[SPRITE_SIZE * SCREEN_WIDTH], 
    dup8((uint8_t) MENU_COLOR_BG),
    SCREEN_WIDTH * (SCREEN_HEIGHT - SPRITE_SIZE) / 2);
}

// TODO: Rename to render menu page
void render_menu(volatile uint8_t *frame_buf, 
  unsigned *anim_state,
  const struct menu *menu) {

  char tmpbuf[TMP_BUF_SIZE];

  const uint8_t pagerowcnt = (menu->bott_callback == NULL ? ROW_COUNT : ROW_COUNT - 1);
  // Odd number round up, e.g. 5 -> we want 3
  const uint8_t pagecenter = pagerowcnt / 2;

  const uint8_t selector = *menu->menu_sel;

  // uint8_t titlecnt = 0;

  // for (uint8_t row = 0; row <= selector + titlecnt; row++)
  //     titlecnt += menu->rows[row].type == MENU_ROW_HEADER;

  // selector += titlecnt;

  const uint8_t lastpagebaseopt = (menu->row_cnt <= pagerowcnt ? 0 : menu->row_cnt - pagerowcnt);
  const uint8_t baseopt = MIN(
    (selector <= pagecenter ? 0 : selector - pagecenter ),
    lastpagebaseopt
  );

  // npf_snprintf(tmpbuf, TMP_BUF_SIZE, "%i", selector);
  // draw_text_ovf(tmpbuf, frame_buf, MENU_MARGIN + SUBMENU_BORDER, 23, 64);
    
  const bool scroll = menu->row_cnt > pagerowcnt;
  const bool lastpage = baseopt == lastpagebaseopt;

  const bool popup = menu->type == MENU_POPUP;
  
  const uint8_t offy = (scroll ? TABS_HEIGHT + 7 : TABS_HEIGHT);
  const uint8_t offx = (popup ? 12 : MENU_MARGIN);

  if (popup) {
    draw_box_outline(frame_buf, 
      SUBMENU_BORDER, SCREEN_WIDTH - SUBMENU_BORDER, 
      TOP_BAR_HEIGHT + SUBMENU_BORDER, SCREEN_HEIGHT - SUBMENU_BORDER, 
      MENU_COLOR_FG);
    draw_text_ovf("⯇", frame_buf, MENU_MARGIN + SUBMENU_BORDER, 23, 64);
    draw_rightj_text("⯈", frame_buf, SCREEN_WIDTH - (MENU_MARGIN + SUBMENU_BORDER), 23);
  }

  if (scroll && baseopt)
    draw_central_text("⯅", frame_buf, SCREEN_WIDTH / 2, 15);
  if (scroll && !lastpage)
    draw_central_text("⯆", frame_buf, SCREEN_WIDTH / 2, ROW_HEIGHT * (pagerowcnt + 1));
  
  // Render the highlight bar if not on a button
  // Render before text rendering occurrs so it is layered
  if (menu->rows[selector].type != MENU_ROW_BUTT)
    render_bar(frame_buf, MENU_COLOR_HI, 0, offy + (selector - baseopt) * ROW_HEIGHT, SCREEN_WIDTH, 16);
  
  for (uint8_t row = 0; row < MIN(pagerowcnt, menu->row_cnt); row++)
  {
    const uint16_t curropt = baseopt + row;
    if (curropt >= menu->row_cnt)
      break;
    render_menu_option(
      frame_buf,
      *anim_state, 
      &menu->rows[baseopt + row], 
      selector == baseopt + row, 
      offx,
      offy + row * ROW_HEIGHT
    );

  }
  
  // Render the bottom bar for Helptext or additional context
  if (menu->bott_callback != NULL){
    menu->bott_callback(tmpbuf, TMP_BUF_SIZE, selector, menu->context);
    if (popup){
      render_bar(
        frame_buf, MENU_COLOR_FG, 
        offx, SCREEN_HEIGHT - ROW_HEIGHT - SUBMENU_BORDER, 
        SCREEN_WIDTH - offx * 2, 16
      );
      render_scrollable_text(
        tmpbuf, frame_buf, 
        offx, SCREEN_HEIGHT - ROW_HEIGHT - SUBMENU_BORDER, 
        SCREEN_WIDTH - offx * 2, anim_state
      );
    } else{
      render_bar(
        frame_buf, MENU_COLOR_FG,
        0, SCREEN_HEIGHT - ROW_HEIGHT,
        SCREEN_WIDTH, ROW_HEIGHT
      );
      draw_text_ovf_rotate(
        tmpbuf, frame_buf,
        offx, SCREEN_HEIGHT - 18,
        SCREEN_WIDTH - offx * 2,
        anim_state
      );
    }
  }
}

