#ifndef REMOTE_GUI_H
#define REMOTE_GUI_H

#include "global.h"
#include "storage.h"
#include "menu.h"

enum {
	MENU_PROMO_IMAGE,
	MENU_REMOTE_ON,
	RADIO_RX_ID,
	RADIO_TX_ID,
	MENU_EMPTY_LINE,
	MENU_PROMO_STATS,
	MENU_PROMO_STATS_V,
	MENU_PROMO_STATS_T,
	MENU_NVM_RESET,
};

//! FIXME: This is not clean
extern menu_item_t menus[];


extern void remote_gui_init(user_storage_t *user);

#endif