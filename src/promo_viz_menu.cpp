#include "promo_viz_menu.h"
#include "remote_gui.h"

#include "promo_outline.h"



static uint16_t sensors_distance[4] = {0};



extern void menu_promo_set_distances(uint16_t* distances) {
	memcpy(sensors_distance, distances, 4*sizeof(uint16_t));
	Serial.print("L:"); Serial.print(sensors_distance[1]); Serial.print(", ");
	Serial.print("F:"); Serial.print(sensors_distance[0]); Serial.print(", ");
	Serial.print("D:"); Serial.print(sensors_distance[2]); Serial.print(", ");
	Serial.print("B:"); Serial.print(sensors_distance[3]); Serial.println(" Update");
	menu_min_redraw(&menus[MENU_PROMO_IMAGE], PARTIAL_REDRAW);
}


extern void menu_promo_image_draw(Adafruit_ST7735 *screen, menu_item_t *menu) {
    char buffer[6];
    uint16_t start_time = millis();

    if(menu->redraw == FULL_REDRAW) {
        draw_image_centered(90, 70, promo_outline_array, PROMO_OUTLINE_WIDTH, PROMO_OUTLINE_HEIGHT);
    }

    screen->setTextColor(WHITE, BLACK);

    //! RIGHT
    if(sensors_distance[2] > 999) {
    	snprintf(buffer, 4, "%3s", "INF");
    }
    else {
        snprintf(buffer, 4, "%3u", sensors_distance[2]);
    }
    screen->setCursor(35, 35);
    screen->print(buffer);

    //! FRONT
    if(sensors_distance[0] > 999) {
        snprintf(buffer, 4, "%3s", "INF");
    }
    else {
        snprintf(buffer, 4, "%3u", sensors_distance[0]);
    }
    screen->setCursor(25, 65);
    screen->print(buffer);

    //! LEFT
    if(sensors_distance[1] > 999) {
        snprintf(buffer, 4, "%3s", "INF");
    }
    else {
        snprintf(buffer, 4, "%3u", sensors_distance[1]);
    }
    screen->setCursor(35, 95);
    screen->print(buffer);

    //! BACK
    if(sensors_distance[3] > 999) {
        snprintf(buffer, 4, "%3s", "INF");
    }
    else {
        snprintf(buffer, 4, "%3u", sensors_distance[3]);
    }
    screen->setCursor(140, 65);
    screen->print(buffer);

    uint16_t draw_time = millis() - start_time;
    screen->setCursor(3, 15);
    if(draw_time > 999) { screen->print("999+ms"); }
    else { 
        snprintf(buffer, 6, "%ums", draw_time);
        screen->print(buffer);
    }
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
