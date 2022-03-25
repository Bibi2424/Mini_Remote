#ifndef PROMO_VIZ_MENU_H
#define PROMO_VIZ_MENU_H

#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "menu.h"


extern void menu_promo_set_distances(uint16_t* distances);
extern void menu_promo_image_draw(Adafruit_ST7735 *screen, menu_item_t *menu);
extern menu_item_t* menu_promo_image_navigate(NAVIGATE_OPTIONS_t action, menu_item_t *menu);




#endif