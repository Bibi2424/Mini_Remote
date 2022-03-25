#include "promo_viz_menu.h"
#include "remote_gui.h"

#include "promo_outline.h"


static uint16_t sensors_distance[4] = {0};


static void clamp_sensor_distance(uint16_t *distances) {
	for(uint8_t i = 0; i < 4; i++) {
		if(distances[i] >= 1000) { distances[i] = 999U; }
	}
}


extern void menu_promo_set_distances(uint16_t* distances) {
	memcpy(sensors_distance, distances, 4*sizeof(uint16_t));
	clamp_sensor_distance(sensors_distance);
	Serial.print("L:"); Serial.print(sensors_distance[1]); Serial.print(", ");
	Serial.print("F:"); Serial.print(sensors_distance[0]); Serial.print(", ");
	Serial.print("D:"); Serial.print(sensors_distance[2]); Serial.print(", ");
	Serial.print("B:"); Serial.print(sensors_distance[3]); Serial.println(" Update");
	menu_min_redraw(&menus[MENU_PROMO_IMAGE], PARTIAL_REDRAW);
}


extern void menu_promo_image_draw(Adafruit_ST7735 *screen, menu_item_t *menu) {

    uint16_t time = millis();

    if(menu->redraw == FULL_REDRAW) {
        draw_image_centered(90, 70, promo_outline_array, PROMO_OUTLINE_WIDTH, PROMO_OUTLINE_HEIGHT);
    }

    // RIGHT
    screen->setCursor(35, 35);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(35, 35);
    screen->print(sensors_distance[2]);

    // FRONT
    screen->setCursor(25, 65);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(25, 65);
    screen->print(sensors_distance[0]);

    // LEFT
    screen->setCursor(35, 95);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(35, 95);
    screen->print(sensors_distance[1]);

    // BACK
    screen->setCursor(140, 65);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(140, 65);
    screen->print(sensors_distance[3]);

    time = millis() - time;
    tft.setCursor(3, 15);
    tft.print(time);
    tft.print("ms");
}


extern menu_item_t* menu_promo_image_navigate(NAVIGATE_OPTIONS_t action, menu_item_t *menu) {
    switch(action) {
        case NAVIGATE_UP:
            // break;
        case NAVIGATE_DOWN:
            // break;
        case NAVIGATE_LEFT:
            // break;
        case NAVIGATE_RIGHT:
            break;
        case NAVIGATE_ENTER:
            if(menu->parent != NULL) {
                menu = menu->parent;
                menu->redraw = FULL_REDRAW;
            }
            break;
        default:
            break;
    }
    return menu;
}
