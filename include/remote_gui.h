#ifndef REMOTE_GUI_H
#define REMOTE_GUI_H

#include "global.h"
#include "storage.h"
#include "menu.h"

#define MENU_PROMO_IMAGE    0
#define RADIO_RX_ID         1
#define RADIO_TX_ID         2
#define MENU_EMPTY_LINE     3
#define MENU_PROMO_STATS    4
#define MENU_PROMO_STATS_V    5
#define MENU_PROMO_STATS_T    6

//! FIXME: This is not clean
extern menu_item_t menus[];


extern void remote_gui_init(user_storage_t *user);

#endif